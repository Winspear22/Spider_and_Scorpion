/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserPNG.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:00:43 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParserPNG.hpp"

/* Forme canonique */
ExifParserPNG::ExifParserPNG()
{
	std::cout << "ExifParserPNG constructor called." << std::endl;
}

ExifParserPNG::~ExifParserPNG()
{
	std::cout << "ExifParserPNG destructor called." << std::endl;
}

ExifParserPNG::ExifParserPNG(const ExifParserPNG &other)
{
	*this = other;
	std::cout << "ExifParserPNG copy constructor called." << std::endl;
}

const ExifParserPNG &ExifParserPNG::operator=(const ExifParserPNG &other)
{
	if (this != &other)
		(void)other;
	std::cout << "ExifParserPNG copy assignment operator called." << std::endl;
	return *this;
}

/* ========================================================================== */
/*                         PARSING PNG                                       */
/* ========================================================================== */

/*
 * parse() — Extrait les métadonnées d'un fichier PNG.
 *
 * Structure PNG :
 *   [8] Signature PNG
 *   Puis des "chunks" :
 *     [4] longueur (big-endian)
 *     [4] type (4 chars ASCII : IHDR, tEXt, IEND...)
 *     [N] données
 *     [4] CRC
 *
 * IHDR (1er chunk) : largeur, hauteur, bit depth, color type
 * tEXt : keyword\0texte (Author, Comment, etc.)
 */
void ExifParserPNG::parse(const std::vector<unsigned char> &data)
{
	std::cout << "  --- Métadonnées PNG ---" << std::endl;
	
	/* Le 1er chunk commence après la signature PNG (8 octets) */
	size_t pos = 8;

	while (pos + 12 <= data.size())
	{
    	/* Longueur du chunk (PNG = toujours big-endian) */
    	uint32_t chunkLen = _read32(data, pos, true);

    	/* Type du chunk (4 caractères) */
    	if (!_isOffsetSafe(data, pos + 4, 4))
    		break;
    	std::string chunkType;
    	chunkType += (char)data[pos + 4];
    	chunkType += (char)data[pos + 5];
    	chunkType += (char)data[pos + 6];
    	chunkType += (char)data[pos + 7];

    	size_t chunkDataStart = pos + 8;

    	/* ---- IHDR : dimensions de l'image ---- */
    	if (chunkType == "IHDR" && _isOffsetSafe(data, chunkDataStart, 13))
		{
      		uint32_t width = _read32(data, chunkDataStart, true);
      		uint32_t height = _read32(data, chunkDataStart + 4, true);
      		uint8_t bitDepth = data[chunkDataStart + 8];
      		uint8_t colorType = data[chunkDataStart + 9];

      		std::cout << "  Dimensions        : " << width << " x " << height
                << std::endl;
      		std::cout << "  Bit Depth         : " << (int)bitDepth << std::endl;

      		std::string colorStr = "Unknown";
      		if (colorType == 0)
        		colorStr = "Grayscale";
      		else if (colorType == 2)
        		colorStr = "RGB";
      		else if (colorType == 3)
        		colorStr = "Palette";
      		else if (colorType == 4)
        		colorStr = "Grayscale+Alpha";
      		else if (colorType == 6)
        		colorStr = "RGBA";
      		std::cout << "  Color Type        : " << colorStr << std::endl;
    	}

    	/* ---- tEXt : métadonnées textuelles ---- */
    	if (chunkType == "tEXt" && _isOffsetSafe(data, chunkDataStart, chunkLen))
		{
      		/* Format : keyword\0texte */
      		std::string keyword;
      		size_t j = 0;
      		while (j < chunkLen)
			{
        		if (data[chunkDataStart + j] == 0)
          			break;
        		keyword += (char)data[chunkDataStart + j];
        		j++;
      		}
      		j++; /* sauter le \0 */

      		std::string textValue;
      		while (j < chunkLen)
			{
        		textValue += (char)data[chunkDataStart + j];
        		j++;
      		}

      		if (!keyword.empty())
			{
        		std::cout << "  " << std::left << std::setw(18) << keyword << ": "
                  << textValue << std::endl;
      		}
    	}

    	/* IEND = fin du fichier */
    	if (chunkType == "IEND")
      		break;

    	/* Chunk suivant : 4(len) + 4(type) + data + 4(CRC) */
    	pos = pos + 12 + chunkLen;
	}
}
