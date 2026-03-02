/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:03:45 by adnen             #+#    #+#             */
/*   Updated: 2026/03/03 00:12:21 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXIFPARSER_HPP__
#define __EXIFPARSER_HPP__

#include "includes.hpp"

class ExifParser {
public:
  ExifParser();
  ~ExifParser();
  const ExifParser &operator=(const ExifParser &other);
  ExifParser(const ExifParser &other);

  /* Point d'entrée public */
  static void displayExifData(const std::string &filename);

private:
  /* Sécurité : vérification des limites avant toute lecture */
  static bool _isOffsetSafe(const std::vector<unsigned char> &data,
                            size_t offset, size_t bytesNeeded);

  /* Lecture du fichier binaire en mémoire */
  static std::vector<unsigned char> _readFile(const std::string &filename);

  /* Navigation dans la structure JPEG pour trouver l'EXIF */
  static int _findExifOffset(const std::vector<unsigned char> &data);

  /* Détection de l'ordre des octets (endianness) */
  static bool _isBigEndian(const std::vector<unsigned char> &data,
                           size_t offset);

  /* Lecture d'entiers avec gestion du byte order */
  static uint16_t _read16(const std::vector<unsigned char> &data, size_t offset,
                          bool bigEndian);
  static uint32_t _read32(const std::vector<unsigned char> &data, size_t offset,
                          bool bigEndian);

  /* Lecture d'une chaîne ASCII depuis les données binaires */
  static std::string _readString(const std::vector<unsigned char> &data,
                                 size_t offset, size_t maxLen);

  /* Parsing d'un IFD (Image File Directory) */
  static void _parseIFD(const std::vector<unsigned char> &data,
                        size_t tiffStart, size_t ifdOffset, bool bigEndian,
                        const std::string &ifdName);

  /* Formatage de la valeur d'un tag selon son type EXIF */
  static std::string _formatTagValue(const std::vector<unsigned char> &data,
                                     size_t tiffStart, size_t entryValueOffset,
                                     uint16_t type, uint32_t count,
                                     bool bigEndian);

  /* Conversion d'un tag ID en nom lisible */
  static std::string _getTagName(uint16_t tag, const std::string &ifdName);
};

#endif