/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserJPEG.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 11:49:38 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParserJPEG.hpp"

/* Forme canonique */
ExifParserJPEG::ExifParserJPEG()
{
	std::cout << "ExifParserJPEG constructor" << std::endl;
}

ExifParserJPEG::~ExifParserJPEG()
{
	std::cout << "ExifParserJPEG destructor" << std::endl;
}

ExifParserJPEG::ExifParserJPEG(const ExifParserJPEG &other): ExifParser(other)
{
	std::cout << "ExifParserJPEG copy constructor" << std::endl;
	(void)other;
}

const ExifParserJPEG &ExifParserJPEG::operator=(const ExifParserJPEG &other)
{
	std::cout << "ExifParserJPEG assignment operator" << std::endl;
  (void)other;
  return *this;
}

/* ========================================================================== */
/*                     RECHERCHE DU SEGMENT EXIF                             */
/* ========================================================================== */

/*
 * _findExifOffset() — Parcourt les segments JPEG pour trouver APP1.
 *
 * Structure JPEG :
 *   FF D8         → SOI (Start Of Image)
 *   FF E1 [len]   → APP1 (contient "Exif\0\0" puis le header TIFF)
 *
 * Retourne l'offset du header TIFF, ou -1 si pas d'EXIF.
 */
int ExifParserJPEG::_findExifOffset(const std::vector<unsigned char> &data) {
  if (!_isOffsetSafe(data, 0, 2))
    return -1;
  if (data[0] != 0xFF || data[1] != 0xD8)
    return -1;

  size_t pos = 2;

  while (pos + 4 < data.size()) {
    if (data[pos] != 0xFF)
      return -1;

    uint8_t marker = data[pos + 1];

    /* SOS = fin des métadonnées */
    if (marker == 0xDA)
      return -1;

    if (!_isOffsetSafe(data, pos + 2, 2))
      return -1;
    uint16_t segLen = (data[pos + 2] << 8) | data[pos + 3];

    /* APP1 (0xE1) contient l'EXIF */
    if (marker == 0xE1 && _isOffsetSafe(data, pos + 4, 6)) {
      if (data[pos + 4] == 'E' && data[pos + 5] == 'x' &&
          data[pos + 6] == 'i' && data[pos + 7] == 'f' &&
          data[pos + 8] == 0x00 && data[pos + 9] == 0x00) {
        return (int)(pos + 10);
      }
    }

    pos = pos + 2 + segLen;
  }
  return -1;
}

/* ========================================================================== */
/*                          BYTE ORDER                                       */
/* ========================================================================== */

/* "MM" = Big Endian (Motorola), "II" = Little Endian (Intel) */
bool ExifParserJPEG::_detectByteOrder(const std::vector<unsigned char> &data,
                                      size_t offset) {
  if (!_isOffsetSafe(data, offset, 2))
    return false;
  return (data[offset] == 'M' && data[offset + 1] == 'M');
}

/* ========================================================================== */
/*                       LECTURE D'UN TAG ASCII                              */
/* ========================================================================== */

/*
 * _readAsciiTag() — Lit un tag EXIF de type ASCII (type 2).
 * Si count <= 4 : la donnée est inline dans l'entrée IFD.
 * Sinon         : c'est un offset vers les données réelles.
 */
std::string
ExifParserJPEG::_readAsciiTag(const std::vector<unsigned char> &data,
                              size_t tiffStart, size_t entryValueOffset,
                              uint32_t count, bool bigEndian) {
  size_t dataOffset;

  if (count <= 4)
    dataOffset = entryValueOffset;
  else {
    uint32_t rawOffset = _read32(data, entryValueOffset, bigEndian);
    dataOffset = tiffStart + rawOffset;
  }

  if (!_isOffsetSafe(data, dataOffset, count))
    return "";

  std::string result;
  size_t i = 0;
  while (i < count) {
    if (data[dataOffset + i] == 0)
      break;
    result += static_cast<char>(data[dataOffset + i]);
    i++;
  }
  return result;
}

/* ========================================================================== */
/*                         PARSING D'UN IFD                                  */
/* ========================================================================== */

/*
 * _parseIFD() — Parcourt un IFD et affiche les tags EXIF basiques.
 *
 * Tags IFD0 :
 *   0x010F = Make, 0x0110 = Model, 0x0131 = Software
 *   0x0132 = DateTime, 0x013B = Artist, 0x8298 = Copyright
 *   0x8769 = pointeur vers Exif Sub-IFD
 *
 * Tags Exif Sub-IFD :
 *   0x9003 = DateTimeOriginal, 0x9004 = DateTimeDigitized
 */
void ExifParserJPEG::_parseIFD(const std::vector<unsigned char> &data,
                               size_t tiffStart, size_t ifdOffset,
                               bool bigEndian, bool isExifSubIFD) {
  size_t absOffset = tiffStart + ifdOffset;

  if (!_isOffsetSafe(data, absOffset, 2))
    return;

  uint16_t entryCount = _read16(data, absOffset, bigEndian);
  if (entryCount > 500)
    return;

  size_t entryBase = absOffset + 2;
  uint16_t idx = 0;

  while (idx < entryCount) {
    size_t entryPos = entryBase + idx * 12;

    if (!_isOffsetSafe(data, entryPos, 12))
      break;

    uint16_t tag = _read16(data, entryPos, bigEndian);
    uint16_t type = _read16(data, entryPos + 2, bigEndian);
    uint32_t count = _read32(data, entryPos + 4, bigEndian);

    /* On ne traite que les tags ASCII (type 2) */
    if (type == 2) {
      std::string value =
          _readAsciiTag(data, tiffStart, entryPos + 8, count, bigEndian);

      if (!value.empty()) {
        if (!isExifSubIFD) {
          if (tag == 0x010F)
            std::cout << "  Make              : " << value << std::endl;
          if (tag == 0x0110)
            std::cout << "  Model             : " << value << std::endl;
          if (tag == 0x0131)
            std::cout << "  Software          : " << value << std::endl;
          if (tag == 0x0132)
            std::cout << "  DateTime          : " << value << std::endl;
          if (tag == 0x013B)
            std::cout << "  Artist            : " << value << std::endl;
          if (tag == 0x8298)
            std::cout << "  Copyright         : " << value << std::endl;
        }
        if (isExifSubIFD) {
          if (tag == 0x9003)
            std::cout << "  DateTimeOriginal  : " << value << std::endl;
          if (tag == 0x9004)
            std::cout << "  DateTimeDigitized : " << value << std::endl;
        }
      }
    }

    /* Pointeur vers le sous-IFD Exif */
    if (tag == 0x8769 && !isExifSubIFD) {
      uint32_t subOffset = _read32(data, entryPos + 8, bigEndian);
      _parseIFD(data, tiffStart, subOffset, bigEndian, true);
    }

    idx++;
  }
}

/* ========================================================================== */
/*                          POINT D'ENTRÉE                                   */
/* ========================================================================== */

void ExifParserJPEG::parse(const std::vector<unsigned char> &data) {
  int tiffStart = _findExifOffset(data);
  if (tiffStart < 0) {
    std::cout << "  Aucune donnée EXIF trouvée." << std::endl;
    return;
  }

  bool bigEndian = _detectByteOrder(data, tiffStart);

  uint16_t magic = _read16(data, tiffStart + 2, bigEndian);
  if (magic != 0x002A) {
    std::cerr << "  TIFF magic invalide." << std::endl;
    return;
  }

  std::cout << "  --- Données EXIF ---" << std::endl;
  uint32_t ifd0Offset = _read32(data, tiffStart + 4, bigEndian);
  _parseIFD(data, tiffStart, ifd0Offset, bigEndian, false);
}
