/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserBMP.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 18:26:38 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParserBMP.hpp"

/* Forme canonique */
ExifParserBMP::ExifParserBMP()
{
	std::cout << "ExifParserBMP constructor" << std::endl;
}
ExifParserBMP::~ExifParserBMP()
{
	std::cout << "ExifParserBMP destructor" << std::endl;
}

ExifParserBMP::ExifParserBMP(const ExifParserBMP &other): ExifParser(other)
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
void ExifParserBMP::parse(const std::vector<unsigned char> &data)
{
	std::cout << "  --- Métadonnées BMP ---" << std::endl;

	if (!_isOffsetSafe(data, 14, 40)) // Vérifie si le fichier est assez grand pour contenir le header BMP
	{
    	std::cerr << "  Fichier BMP trop court." << std::endl;
    	return;
	}

	/* Dimensions (signed, little-endian) */
	int32_t width = static_cast<int32_t>(_read32(data, 18, false)); // Lecture de la largeur du fichier BMP
	int32_t height = static_cast<int32_t>(_read32(data, 22, false)); // Lecture de la hauteur du fichier BMP
	if (height < 0) // Si la hauteur est négative, on prend sa valeur absolue (sans le signe négatif)
		height = -height; // Sinon on la laisse telle quelle (les fichiers BMP peuvent compter la hauteur de bas en haut c'est pour ça qu'on a une hauteur négative)

	std::cout << "  Dimensions        : " << width << " x " << height
            << std::endl;

	/* Profondeur de couleur */
	uint16_t bitsPerPixel = _read16(data, 28, false); // 1 bit = noir et blanc, 8 bits = niveaux de gris, 24 bits = couleur, 32 bits = couleur avec alpha (transparence)
	std::cout << "  Pixel (couleur)   : " << bitsPerPixel << std::endl; 

	/* Compression */
	uint32_t compression = _read32(data, 30, false); // Compression de l'image
	std::string compStr = "Unknown";
	if (compression == 0)
		compStr = "None (BI_RGB)"; // Pas de compression
	else if (compression == 1)
		compStr = "RLE8"; // Compression RLE 8 bits
	else if (compression == 2)
		compStr = "RLE4"; // Compression RLE 4 bits (c'est plus compressé que 8 bits)
	else if (compression == 3)
		compStr = "Bitfields"; // Compression par masques de bits (on choisit le nombre de bits de compression pour chaque couleur)
	std::cout << "  Compression       : " << compStr << std::endl;
}
