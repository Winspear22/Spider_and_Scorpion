/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserBMP.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:03:06 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParserBMP.hpp"

/* Forme canonique */
ExifParserBMP::ExifParserBMP() {}
ExifParserBMP::~ExifParserBMP() {}

ExifParserBMP::ExifParserBMP(const ExifParserBMP &other)
{
	std::cout << "ExifParserBMP copy constructor called." << std::endl;
	*this = other;
}

const ExifParserBMP &ExifParserBMP::operator=(const ExifParserBMP &other)
{
	if (this != &other)
		(void)other;
	std::cout << "ExifParserBMP copy assignment operator called." << std::endl;
	return *this;
}

/* ========================================================================== */
/*                         PARSING BMP                                       */
/* ========================================================================== */

/*
 * parse() — Extrait les métadonnées d'un fichier BMP.
 *
 * Structure BMP :
 *   [14] File Header : "BM" + taille fichier + réservé + offset pixels
 *   [40] DIB Header  (BITMAPINFOHEADER, tout en little-endian) :
 *     offset 14 : [4] taille du header (= 40)
 *     offset 18 : [4] largeur (signé)
 *     offset 22 : [4] hauteur (signé, négatif = top-down)
 *     offset 26 : [2] nombre de plans
 *     offset 28 : [2] bits par pixel
 *     offset 30 : [4] type de compression
 */
void ExifParserBMP::parse(const std::vector<unsigned char> &data) {
  std::cout << "  --- Métadonnées BMP ---" << std::endl;

  if (!_isOffsetSafe(data, 14, 40)) {
    std::cerr << "  Fichier BMP trop court." << std::endl;
    return;
  }

  /* Dimensions (signed, little-endian) */
  int32_t width = (int32_t)_read32(data, 18, false);
  int32_t height = (int32_t)_read32(data, 22, false);
  if (height < 0)
    height = -height;

  std::cout << "  Dimensions        : " << width << " x " << height
            << std::endl;

  /* Bits par pixel */
  uint16_t bitsPerPixel = _read16(data, 28, false);
  std::cout << "  Bits/Pixel        : " << bitsPerPixel << std::endl;

  /* Compression */
  uint32_t compression = _read32(data, 30, false);
  std::string compStr = "Unknown";
  if (compression == 0)
    compStr = "None (BI_RGB)";
  else if (compression == 1)
    compStr = "RLE8";
  else if (compression == 2)
    compStr = "RLE4";
  else if (compression == 3)
    compStr = "Bitfields";
  std::cout << "  Compression       : " << compStr << std::endl;
}
