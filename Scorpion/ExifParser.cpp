/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:03:42 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 17:22:01 by adnen            ###   ########.fr       */
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
 * Le mot Offset veut dire "DISTANCE en mémoire"
 */
bool ExifParser::_isOffsetSafe(const std::vector<unsigned char> &data, size_t offset, size_t bytesNeeded)
{
	if (offset + bytesNeeded < offset) // Sert à contrer le "Wrap around" (dépassement de capacité) --> un hacker donne un fichier qui dépasse un INT (donc le compteur repart à 0)
		return false;
	if (offset + bytesNeeded > data.size()) // Sert à empêcher de lire au-delà du fichier
		return false;
	return true;
}

/*
 * _readFile() — Lit tout le fichier en mémoire (max 50 Mo).
 */
std::vector<unsigned char> ExifParser::_readFile(const std::string &filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate); // On ouvre le fichier en binaire et on met le curseur à la fin pour obtenir sa taille SANS LE PARCOURIR (gain de vitesse)
	if (!file.is_open())
		return std::vector<unsigned char>();

	std::streamsize size = file.tellg(); // On récupère la taille du fichier (le curseur étant à la fin)
	file.seekg(0, std::ios::beg); // On remet le curseur au début

	if (size <= 0 || size > 50 * 1024 * 1024) // Protection contre les fichiers vides ou trop gros
		return std::vector<unsigned char>();

	std::vector<unsigned char> data(size); // On crée un vecteur de la taille du fichier exacte
	file.read(reinterpret_cast<char *>(data.data()), size); // Read est une fonction héritée du C qui attend un char, or on a un vecteur d'unsigned char, donc on doit caster, on utilise CE cast car les autres refusent (car unsigned / signed est de la gestion de signe est dangereux)
	return data;
}

/* Lit 2 octets dans le bon ordre */
uint16_t ExifParser::_read16(const std::vector<unsigned char> &data, size_t offset, bool bigEndian)
{
	if (!_isOffsetSafe(data, offset, 2))  // Sécurité : On vérifie qu'on a bien 2 octets disponibles sous le curseur
		return 0;
	if (bigEndian) // Si bigEndian est vrai, on lit les octets dans l'ordre big-endian
		return (static_cast<uint16_t>(data[offset]) << 8) | static_cast<uint16_t>(data[offset + 1]); // On décale le premier octet de 8 bits vers la gauche (on le met à gauche) et on fait un OU logique avec le deuxième octet (le << est un opérateur de décalage)
	return static_cast<uint16_t>(data[offset]) | (static_cast<uint16_t>(data[offset + 1]) << 8);  // Sinon, on lit dans l'ordre little-endian (on met le deuxième octet à gauche)
}

/* Lit 4 octets dans le bon ordre */
uint32_t ExifParser::_read32(const std::vector<unsigned char> &data, size_t offset, bool bigEndian)
{
	if (!_isOffsetSafe(data, offset, 4)) // Sécurité : On vérifie qu'on a bien 4 octets disponibles sous le curseur
		return 0;
	if (bigEndian)
		return (static_cast<uint32_t>(data[offset]) << 24) | (static_cast<uint32_t>(data[offset + 1]) << 16) |
			   (static_cast<uint32_t>(data[offset + 2]) << 8) | static_cast<uint32_t>(data[offset + 3]);
	return static_cast<uint32_t>(data[offset]) | (static_cast<uint32_t>(data[offset + 1]) << 8) |
		   (static_cast<uint32_t>(data[offset + 2]) << 16) | (static_cast<uint32_t>(data[offset + 3]) << 24);
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
	if (data.size() >= 3 && data[0] == 0xFF && data[1] == 0xD8 && data[2] == 0xFF) // Si la taille est supérieure ou égale à 3 octets 
																					// et que les 3 premiers octets sont FF D8 FF
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
