/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserGIF.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:03:24 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParserGIF.hpp"

/* Forme canonique */
ExifParserGIF::ExifParserGIF() {}
ExifParserGIF::~ExifParserGIF() {}

ExifParserGIF::ExifParserGIF(const ExifParserGIF &other)
{
	*this = other;
}

const ExifParserGIF &ExifParserGIF::operator=(const ExifParserGIF &other)
{
	if (this != &other)
		(void)other;
	std::cout << "ExifParserGIF copy assignment operator called." << std::endl;
	return *this;
}

/* ========================================================================== */
/*                         PARSING GIF                                       */
/* ========================================================================== */

/*
 * parse() — Extrait les métadonnées d'un fichier GIF.
 *
 * Structure du header GIF :
 *   [6] Signature : "GIF87a" ou "GIF89a"
 *   [2] Largeur  (little-endian)
 *   [2] Hauteur  (little-endian)
 *   [1] Packed byte :
 *       bit 7     = palette globale présente
 *       bits 0-2  = taille de la palette (2^(n+1) couleurs)
 */
void ExifParserGIF::parse(const std::vector<unsigned char> &data) {
  std::cout << "  --- Métadonnées GIF ---" << std::endl;

  if (!_isOffsetSafe(data, 0, 13)) {
    std::cerr << "  Fichier GIF trop court." << std::endl;
    return;
  }

  /* Version (GIF87a ou GIF89a) */
  std::string version;
  size_t i = 0;
  while (i < 6) {
    version += (char)data[i];
    i++;
  }
  std::cout << "  Version           : " << version << std::endl;

  /* Dimensions (little-endian) */
  uint16_t width = _read16(data, 6, false);
  uint16_t height = _read16(data, 8, false);
  std::cout << "  Dimensions        : " << width << " x " << height
            << std::endl;

  /* Palette globale */
  uint8_t packed = data[10];
  bool hasGlobalPalette = (packed & 0x80) != 0;
  if (hasGlobalPalette) {
    int colorBits = (packed & 0x07) + 1;
    int numColors = 1;
    int k = 0;
    while (k < colorBits) {
      numColors = numColors * 2;
      k++;
    }
    std::cout << "  Couleurs          : " << numColors << std::endl;
  }
}
