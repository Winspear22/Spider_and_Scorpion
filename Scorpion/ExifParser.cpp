/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:03:42 by adnen             #+#    #+#             */
/*   Updated: 2026/03/03 00:13:56 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParser.hpp"

ExifParser::ExifParser() {
  std::cout << "ExifParser constructor called." << std::endl;
}

ExifParser::~ExifParser() {
  std::cout << "ExifParser destructor called." << std::endl;
}

const ExifParser &ExifParser::operator=(const ExifParser &other) {
  if (this != &other) {
  }
  std::cout << "ExifParser copy assignment operator called." << std::endl;
  return *this;
}

ExifParser::ExifParser(const ExifParser &other) {
  *this = other;
  std::cout << "ExifParser copy constructor called." << std::endl;
}

/* ========================================================================== */
/*                     SÉCURITÉ : VÉRIFICATION DES LIMITES                    */
/* ========================================================================== */

/*
 * _isOffsetSafe() — Vérifie qu'on ne lit PAS en dehors du fichier.
 * C'est la fonction la plus IMPORTANTE pour la sécurité.
 * Chaque lecture binaire DOIT passer par cette vérification.
 */
bool ExifParser::_isOffsetSafe(const std::vector<unsigned char> &data,
                               size_t offset, size_t bytesNeeded) {
  /* Protection contre l'overflow arithmétique */
  if (offset + bytesNeeded < offset)
    return false;
  /* Protection contre la lecture hors limites */
  if (offset + bytesNeeded > data.size())
    return false;
  return true;
}

/* ========================================================================== */
/*                         LECTURE DU FICHIER BINAIRE                         */
/* ========================================================================== */

/*
 * _readFile() — Lit le fichier entièrement en mémoire.
 * Retourne un vecteur vide si le fichier est trop gros ou illisible.
 * Limite : 50 Mo max pour éviter l'épuisement de la mémoire.
 */
std::vector<unsigned char> ExifParser::_readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "  ExifParser: Could not open " << filename << std::endl;
    return std::vector<unsigned char>();
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  /* Protection contre les fichiers vides ou trop gros (50 Mo) */
  if (size <= 0 || size > 50 * 1024 * 1024) {
    std::cerr << "  ExifParser: File too large or empty." << std::endl;
    return std::vector<unsigned char>();
  }

  std::vector<unsigned char> data(size);
  file.read(reinterpret_cast<char *>(data.data()), size);
  return data;
}

/* ========================================================================== */
/*                    NAVIGATION DANS LA STRUCTURE JPEG                       */
/* ========================================================================== */

/*
 * _findExifOffset() — Parcourt les segments JPEG pour trouver APP1 (EXIF).
 * Retourne l'offset du début du header TIFF, ou -1 si non trouvé.
 *
 * Structure JPEG :
 *   FF D8          → SOI (Start Of Image)
 *   FF E1 [len]    → APP1 (contient EXIF)
 *     "Exif\0\0"   → Signature EXIF (6 octets)
 *     [TIFF header] → C'est ici qu'on veut pointer
 */
int ExifParser::_findExifOffset(const std::vector<unsigned char> &data) {
  /* Le JPEG doit commencer par FF D8 */
  if (!_isOffsetSafe(data, 0, 2))
    return -1;
  if (data[0] != 0xFF || data[1] != 0xD8)
    return -1;

  size_t pos = 2;

  /* Parcours segment par segment */
  while (pos + 4 < data.size()) {
    /* Chaque segment JPEG commence par FF XX */
    if (data[pos] != 0xFF)
      return -1;

    uint8_t marker = data[pos + 1];

    /* SOS (Start of Scan) = fin des métadonnées */
    if (marker == 0xDA)
      return -1;

    /* Lecture de la taille du segment (big-endian, toujours) */
    if (!_isOffsetSafe(data, pos + 2, 2))
      return -1;
    uint16_t segLen = (data[pos + 2] << 8) | data[pos + 3];

    /* APP1 (0xE1) = le segment qui contient l'EXIF */
    if (marker == 0xE1) {
      /* Vérifier la signature "Exif\0\0" (6 octets après le marqueur+taille) */
      if (_isOffsetSafe(data, pos + 4, 6)) {
        if (data[pos + 4] == 'E' && data[pos + 5] == 'x' &&
            data[pos + 6] == 'i' && data[pos + 7] == 'f' &&
            data[pos + 8] == 0x00 && data[pos + 9] == 0x00) {
          /* Le header TIFF commence juste après "Exif\0\0" */
          return (int)(pos + 10);
        }
      }
    }

    /* Passer au segment suivant : pos + 2 (marqueur) + segLen */
    pos = pos + 2 + segLen;
  }
  return -1;
}

/* ========================================================================== */
/*                      GESTION DE L'ORDRE DES OCTETS                        */
/* ========================================================================== */

/*
 * _isBigEndian() — Détecte le byte order du TIFF.
 * "MM" (Motorola) = Big Endian
 * "II" (Intel)    = Little Endian
 */
bool ExifParser::_isBigEndian(const std::vector<unsigned char> &data,
                              size_t offset) {
  if (!_isOffsetSafe(data, offset, 2))
    return false;
  if (data[offset] == 'M' && data[offset + 1] == 'M')
    return true;
  return false;
}

/*
 * _read16() — Lit un entier 16 bits avec le bon ordre d'octets.
 * Retourne 0 si la lecture est hors limites.
 */
uint16_t ExifParser::_read16(const std::vector<unsigned char> &data,
                             size_t offset, bool bigEndian) {
  if (!_isOffsetSafe(data, offset, 2))
    return 0;
  if (bigEndian)
    return (data[offset] << 8) | data[offset + 1];
  return data[offset] | (data[offset + 1] << 8);
}

/*
 * _read32() — Lit un entier 32 bits avec le bon ordre d'octets.
 * Retourne 0 si la lecture est hors limites.
 */
uint32_t ExifParser::_read32(const std::vector<unsigned char> &data,
                             size_t offset, bool bigEndian) {
  if (!_isOffsetSafe(data, offset, 4))
    return 0;
  if (bigEndian) {
    return ((uint32_t)data[offset] << 24) | ((uint32_t)data[offset + 1] << 16) |
           ((uint32_t)data[offset + 2] << 8) | (uint32_t)data[offset + 3];
  }
  return (uint32_t)data[offset] | ((uint32_t)data[offset + 1] << 8) |
         ((uint32_t)data[offset + 2] << 16) |
         ((uint32_t)data[offset + 3] << 24);
}

/* ========================================================================== */
/*                       EXTRACTION DE DONNÉES TYPÉES                        */
/* ========================================================================== */

/*
 * _readString() — Lit une chaîne ASCII depuis les données binaires.
 * S'arrête au premier \0 ou à maxLen octets.
 * Vérifie les limites à CHAQUE octet lu.
 */
std::string ExifParser::_readString(const std::vector<unsigned char> &data,
                                    size_t offset, size_t maxLen) {
  std::string result;
  size_t i = 0;
  while (i < maxLen) {
    if (!_isOffsetSafe(data, offset + i, 1))
      break;
    if (data[offset + i] == 0)
      break;
    result += static_cast<char>(data[offset + i]);
    i++;
  }
  return result;
}

/* ========================================================================== */
/*                        NOMS DES TAGS EXIF                                 */
/* ========================================================================== */

/*
 * _getTagName() — Convertit un tag ID numérique en nom lisible.
 * Les tags dépendent de l'IFD dans lequel on se trouve.
 */
std::string ExifParser::_getTagName(uint16_t tag, const std::string &ifdName) {
  /* ---- Tags IFD0 (Image principale) ---- */
  if (ifdName == "IFD0" || ifdName == "IFD1") {
    if (tag == 0x010F)
      return "Make";
    if (tag == 0x0110)
      return "Model";
    if (tag == 0x0112)
      return "Orientation";
    if (tag == 0x011A)
      return "XResolution";
    if (tag == 0x011B)
      return "YResolution";
    if (tag == 0x0128)
      return "ResolutionUnit";
    if (tag == 0x0131)
      return "Software";
    if (tag == 0x0132)
      return "DateTime";
    if (tag == 0x013B)
      return "Artist";
    if (tag == 0x8298)
      return "Copyright";
    if (tag == 0x8769)
      return "ExifIFDPointer";
    if (tag == 0x8825)
      return "GPSInfoIFDPointer";
  }

  /* ---- Tags Exif Sub-IFD (Paramètres photo) ---- */
  if (ifdName == "Exif") {
    if (tag == 0x829A)
      return "ExposureTime";
    if (tag == 0x829D)
      return "FNumber";
    if (tag == 0x8822)
      return "ExposureProgram";
    if (tag == 0x8827)
      return "ISOSpeedRatings";
    if (tag == 0x9000)
      return "ExifVersion";
    if (tag == 0x9003)
      return "DateTimeOriginal";
    if (tag == 0x9004)
      return "DateTimeDigitized";
    if (tag == 0x9201)
      return "ShutterSpeedValue";
    if (tag == 0x9202)
      return "ApertureValue";
    if (tag == 0x9204)
      return "ExposureBiasValue";
    if (tag == 0x9207)
      return "MeteringMode";
    if (tag == 0x9209)
      return "Flash";
    if (tag == 0x920A)
      return "FocalLength";
    if (tag == 0xA001)
      return "ColorSpace";
    if (tag == 0xA002)
      return "PixelXDimension";
    if (tag == 0xA003)
      return "PixelYDimension";
    if (tag == 0xA405)
      return "FocalLengthIn35mm";
    if (tag == 0xA420)
      return "ImageUniqueID";
    if (tag == 0xA434)
      return "LensModel";
  }

  /* ---- Tags GPS ---- */
  if (ifdName == "GPS") {
    if (tag == 0x0000)
      return "GPSVersionID";
    if (tag == 0x0001)
      return "GPSLatitudeRef";
    if (tag == 0x0002)
      return "GPSLatitude";
    if (tag == 0x0003)
      return "GPSLongitudeRef";
    if (tag == 0x0004)
      return "GPSLongitude";
    if (tag == 0x0005)
      return "GPSAltitudeRef";
    if (tag == 0x0006)
      return "GPSAltitude";
    if (tag == 0x0007)
      return "GPSTimeStamp";
    if (tag == 0x001D)
      return "GPSDateStamp";
  }

  /* Tag inconnu : afficher l'ID en hex */
  std::ostringstream oss;
  oss << "Tag_0x" << std::hex << std::setfill('0') << std::setw(4) << tag;
  return oss.str();
}

/* ========================================================================== */
/*                    FORMATAGE DES VALEURS DE TAGS                          */
/* ========================================================================== */

/*
 * _formatTagValue() — Formate la valeur d'un tag EXIF selon son type.
 *
 * Types EXIF :
 *   1 = BYTE       (1 octet)
 *   2 = ASCII      (1 octet/char)
 *   3 = SHORT      (2 octets)
 *   4 = LONG       (4 octets)
 *   5 = RATIONAL   (8 octets : numérateur + dénominateur)
 *   7 = UNDEFINED  (1 octet, binaire)
 *   9 = SLONG      (4 octets, signé)
 *  10 = SRATIONAL  (8 octets, signé)
 *
 * Si totalSize <= 4 octets : la valeur est INLINE dans l'entrée IFD.
 * Si totalSize > 4 octets  : l'entrée contient un OFFSET vers les données.
 */
std::string ExifParser::_formatTagValue(const std::vector<unsigned char> &data,
                                        size_t tiffStart,
                                        size_t entryValueOffset, uint16_t type,
                                        uint32_t count, bool bigEndian) {
  /* Taille de chaque élément selon le type */
  size_t typeSize = 0;
  if (type == 1 || type == 2 || type == 7)
    typeSize = 1;
  else if (type == 3)
    typeSize = 2;
  else if (type == 4 || type == 9)
    typeSize = 4;
  else if (type == 5 || type == 10)
    typeSize = 8;
  else
    return "[type inconnu]";

  /* Protection : count absurde = fichier corrompu */
  if (count > 10000)
    return "[count trop grand]";

  size_t totalSize = count * typeSize;

  /* Déterminer d'où lire les données */
  size_t dataOffset;
  if (totalSize <= 4) {
    /* Données inline : elles sont directement dans le champ valeur */
    dataOffset = entryValueOffset;
  } else {
    /* Données externes : le champ valeur contient un offset */
    uint32_t rawOffset = _read32(data, entryValueOffset, bigEndian);
    dataOffset = tiffStart + rawOffset;
  }

  /* Vérification des limites AVANT toute lecture */
  if (!_isOffsetSafe(data, dataOffset, totalSize))
    return "[données hors limites]";

  /* ---- ASCII (type 2) ---- */
  if (type == 2)
    return _readString(data, dataOffset, count);

  /* ---- BYTE (type 1) ---- */
  if (type == 1) {
    std::ostringstream oss;
    size_t idx = 0;
    while (idx < count) {
      if (idx > 0)
        oss << ".";
      oss << (int)data[dataOffset + idx];
      idx++;
    }
    return oss.str();
  }

  /* ---- SHORT (type 3) ---- */
  if (type == 3) {
    std::ostringstream oss;
    size_t idx = 0;
    while (idx < count) {
      if (idx > 0)
        oss << ", ";
      oss << _read16(data, dataOffset + idx * 2, bigEndian);
      idx++;
    }
    return oss.str();
  }

  /* ---- LONG (type 4) ---- */
  if (type == 4) {
    std::ostringstream oss;
    size_t idx = 0;
    while (idx < count) {
      if (idx > 0)
        oss << ", ";
      oss << _read32(data, dataOffset + idx * 4, bigEndian);
      idx++;
    }
    return oss.str();
  }

  /* ---- RATIONAL (type 5) : numérateur / dénominateur ---- */
  if (type == 5) {
    std::ostringstream oss;
    size_t idx = 0;
    while (idx < count) {
      uint32_t num = _read32(data, dataOffset + idx * 8, bigEndian);
      uint32_t den = _read32(data, dataOffset + idx * 8 + 4, bigEndian);
      if (idx > 0)
        oss << ", ";
      if (den == 0)
        oss << "0";
      else if (num % den == 0)
        oss << (num / den);
      else
        oss << std::fixed << std::setprecision(4)
            << ((double)num / (double)den);
      idx++;
    }
    return oss.str();
  }

  /* ---- SRATIONAL (type 10) : signé ---- */
  if (type == 10) {
    std::ostringstream oss;
    size_t idx = 0;
    while (idx < count) {
      int32_t num = (int32_t)_read32(data, dataOffset + idx * 8, bigEndian);
      int32_t den = (int32_t)_read32(data, dataOffset + idx * 8 + 4, bigEndian);
      if (idx > 0)
        oss << ", ";
      if (den == 0)
        oss << "0";
      else
        oss << std::fixed << std::setprecision(4)
            << ((double)num / (double)den);
      idx++;
    }
    return oss.str();
  }

  /* ---- SLONG (type 9) ---- */
  if (type == 9) {
    std::ostringstream oss;
    oss << (int32_t)_read32(data, dataOffset, bigEndian);
    return oss.str();
  }

  /* ---- UNDEFINED (type 7) : essayer ASCII, sinon hex ---- */
  if (type == 7) {
    /* Vérifier si c'est du texte lisible */
    bool isPrintable = true;
    size_t checkLen = count;
    if (checkLen > 32)
      checkLen = 32;
    size_t j = 0;
    while (j < checkLen) {
      unsigned char c = data[dataOffset + j];
      if (c != 0 && (c < 32 || c > 126)) {
        isPrintable = false;
        break;
      }
      j++;
    }
    if (isPrintable)
      return _readString(data, dataOffset, count);

    /* Sinon, afficher en hexadécimal (max 16 octets) */
    std::ostringstream oss;
    size_t maxShow = count;
    if (maxShow > 16)
      maxShow = 16;
    j = 0;
    while (j < maxShow) {
      if (j > 0)
        oss << " ";
      oss << std::hex << std::setfill('0') << std::setw(2)
          << (int)data[dataOffset + j];
      j++;
    }
    if (count > 16)
      oss << "...";
    return oss.str();
  }

  return "[type non supporté]";
}

/* ========================================================================== */
/*                      PARSING D'UN IFD (DIRECTORY)                         */
/* ========================================================================== */

/*
 * _parseIFD() — Parse un IFD et affiche chaque tag trouvé.
 *
 * Structure d'un IFD :
 *   [2 octets]      Nombre d'entrées (N)
 *   [12 octets × N] Entrées : Tag(2) + Type(2) + Count(4) + Value(4)
 *   [4 octets]      Offset vers le prochain IFD (0 = dernier)
 *
 * Les tags spéciaux 0x8769 et 0x8825 pointent vers des sous-IFD
 * (Exif et GPS) qui sont parsés récursivement.
 */
void ExifParser::_parseIFD(const std::vector<unsigned char> &data,
                           size_t tiffStart, size_t ifdOffset, bool bigEndian,
                           const std::string &ifdName) {
  size_t absOffset = tiffStart + ifdOffset;

  /* Sécurité : vérifier que l'offset est dans le fichier */
  if (!_isOffsetSafe(data, absOffset, 2)) {
    std::cerr << "  [" << ifdName << "] Offset invalide." << std::endl;
    return;
  }

  uint16_t entryCount = _read16(data, absOffset, bigEndian);

  /* Protection contre la corruption : max 500 entrées par IFD */
  if (entryCount > 500) {
    std::cerr << "  [" << ifdName << "] Trop d'entrées (" << entryCount
              << "), fichier corrompu." << std::endl;
    return;
  }

  std::cout << "  --- " << ifdName << " (" << entryCount << " tags) ---"
            << std::endl;

  size_t entryBase = absOffset + 2;
  uint16_t idx = 0;

  while (idx < entryCount) {
    size_t entryPos = entryBase + idx * 12;

    /* Sécurité : chaque entrée fait 12 octets */
    if (!_isOffsetSafe(data, entryPos, 12))
      break;

    uint16_t tag = _read16(data, entryPos, bigEndian);
    uint16_t type = _read16(data, entryPos + 2, bigEndian);
    uint32_t count = _read32(data, entryPos + 4, bigEndian);
    /* entryPos + 8 = début du champ value/offset (4 octets) */

    std::string tagName = _getTagName(tag, ifdName);

    /* Tags spéciaux : pointeurs vers des sous-IFD */
    if (tag == 0x8769) /* → Exif Sub-IFD */
    {
      uint32_t subOffset = _read32(data, entryPos + 8, bigEndian);
      _parseIFD(data, tiffStart, subOffset, bigEndian, "Exif");
    } else if (tag == 0x8825) /* → GPS Sub-IFD */
    {
      uint32_t subOffset = _read32(data, entryPos + 8, bigEndian);
      _parseIFD(data, tiffStart, subOffset, bigEndian, "GPS");
    } else {
      /* Tag normal : lire et afficher la valeur */
      std::string value = _formatTagValue(data, tiffStart, entryPos + 8, type,
                                          count, bigEndian);
      std::cout << "  " << std::left << std::setw(25) << tagName << ": "
                << value << std::endl;
    }

    idx++;
  }
}

/* ========================================================================== */
/*                        POINT D'ENTRÉE PUBLIC                              */
/* ========================================================================== */

/*
 * displayExifData() — Lit un fichier JPEG et affiche ses données EXIF.
 *
 * Pipeline :
 *   1. Charger le fichier en mémoire
 *   2. Trouver le segment APP1 (EXIF)
 *   3. Détecter l'ordre des octets (big/little endian)
 *   4. Vérifier le magic number TIFF (0x002A)
 *   5. Parser l'IFD0 (et les sous-IFD récursivement)
 */
void ExifParser::displayExifData(const std::string &filename) {
  /* 1. Charger le fichier */
  std::vector<unsigned char> data = _readFile(filename);
  if (data.empty())
    return;

  /* 2. Trouver l'EXIF */
  int tiffStart = _findExifOffset(data);
  if (tiffStart < 0) {
    std::cout << "  Aucune donnée EXIF trouvée." << std::endl;
    return;
  }

  /* 3. Détecter le byte order */
  bool bigEndian = _isBigEndian(data, tiffStart);

  /* 4. Vérifier le magic number TIFF */
  uint16_t magic = _read16(data, tiffStart + 2, bigEndian);
  if (magic != 0x002A) {
    std::cerr << "  TIFF magic number invalide." << std::endl;
    return;
  }

  /* 5. Lire l'offset vers l'IFD0 et parser */
  uint32_t ifd0Offset = _read32(data, tiffStart + 4, bigEndian);
  _parseIFD(data, tiffStart, ifd0Offset, bigEndian, "IFD0");
}
