/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:03:42 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:21:14 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExifParser.hpp"
#include "ExifParserBMP.hpp"
#include "ExifParserGIF.hpp"
#include "ExifParserJPEG.hpp"
#include "ExifParserPNG.hpp"

/* ========================================================================== */
/*                         FORME CANONIQUE                                   */
/* ========================================================================== */

ExifParser::ExifParser()
{
  std::cout << "ExifParser constructor" << std::endl;
}
ExifParser::~ExifParser()
{
  std::cout << "ExifParser destructor" << std::endl;
}

ExifParser::ExifParser(const ExifParser &other)
{
  std::cout << "ExifParser copy constructor called." << std::endl;
  *this = other;
}

const ExifParser &ExifParser::operator=(const ExifParser &other)
{	
  if (this != &other)
    (void)other;
  std::cout << "ExifParser copy assignment operator called." << std::endl;
  return *this;
}

/* ========================================================================== */
/*                     FONCTIONS UTILITAIRES COMMUNES                         */
/* ========================================================================== */

/*
 * _isOffsetSafe() — Empêche de lire au-delà du fichier.
 * Chaque lecture binaire DOIT passer par cette vérification.
 */
bool ExifParser::_isOffsetSafe(const std::vector<unsigned char> &data, size_t offset, size_t bytesNeeded)
{
	if (offset + bytesNeeded < offset)
		return false;
	if (offset + bytesNeeded > data.size())
		return false;
	return true;
}

/*
 * _readFile() — Lit tout le fichier en mémoire (max 50 Mo).
 */
std::vector<unsigned char> ExifParser::_readFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return std::vector<unsigned char>();

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0 || size > 50 * 1024 * 1024)
		return std::vector<unsigned char>();

	std::vector<unsigned char> data(size);
	file.read(reinterpret_cast<char *>(data.data()), size);
	return data;
}

/* Lit 2 octets dans le bon ordre */
uint16_t ExifParser::_read16(const std::vector<unsigned char> &data, size_t offset, bool bigEndian)
{
	if (!_isOffsetSafe(data, offset, 2))
		return 0;
	if (bigEndian)
		return (data[offset] << 8) | data[offset + 1];
	return data[offset] | (data[offset + 1] << 8);
}

/* Lit 4 octets dans le bon ordre */
uint32_t ExifParser::_read32(const std::vector<unsigned char> &data, size_t offset, bool bigEndian)
{
	if (!_isOffsetSafe(data, offset, 4))
		return 0;
	if (bigEndian)
		return ((uint32_t)data[offset] << 24) | ((uint32_t)data[offset + 1] << 16) |
			   ((uint32_t)data[offset + 2] << 8) | (uint32_t)data[offset + 3];
	return (uint32_t)data[offset] | ((uint32_t)data[offset + 1] << 8) |
		   ((uint32_t)data[offset + 2] << 16) | ((uint32_t)data[offset + 3] << 24);
}

/* ========================================================================== */
/*                     FACTORY : DÉTECTION ET DISPATCH                       */
/* ========================================================================== */

/*
 * displayExifData() — Détecte le format du fichier via ses magic numbers
 * et instancie le bon parser (héritage).
 */
void ExifParser::displayExifData(const std::string &filename)
{
	std::vector<unsigned char> data = _readFile(filename);
	if (data.empty())
		return;

	/* JPEG : FF D8 FF */
	if (data.size() >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF)
	{
		ExifParserJPEG parser;
		parser.parse(data);
		return;
	}

  	/* PNG : 89 50 4E 47 */
  	if (data.size() >= 4 && data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47)
	{
    	ExifParserPNG parser;
    	parser.parse(data);
    	return;
  	}
	
	/* GIF : "GIF8" */
  	if (data.size() >= 4 && data[0] == 0x47 && data[1] == 0x49 && data[2] == 0x46 && data[3] == 0x38)
	{
    	ExifParserGIF parser;
    	parser.parse(data);
    	return;
  	}
	
	/* BMP : "BM" */
  	if (data.size() >= 2 && data[0] == 0x42 && data[1] == 0x4D)
	{
    	ExifParserBMP parser;
    	parser.parse(data);
    	return;
  	}
	std::cout << "  Format non supporté." << std::endl;
}
