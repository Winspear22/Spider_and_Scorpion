/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:03:45 by adnen             #+#    #+#             */
/*   Updated: 2026/03/03 00:36:52 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXIFPARSER_HPP__
#define __EXIFPARSER_HPP__

#include "includes.hpp"

/*
 * Classe de base abstraite pour le parsing de métadonnées.
 * Chaque format (JPEG, PNG, GIF, BMP) hérite de cette classe
 * et implémente sa propre méthode parse().
 *
 * Les fonctions utilitaires communes (lecture binaire, vérification
 * des limites) sont partagées via l'héritage (protected).
 */
class ExifParser
{
public:
  ExifParser();
  virtual ~ExifParser();
  ExifParser(const ExifParser &other);
  const ExifParser &operator=(const ExifParser &other);

  /* Factory : détecte le format et appelle le bon parser */
  static void displayExifData(const std::string &filename);

  /* Méthode virtuelle pure : chaque format l'implémente */
  virtual void parse(const std::vector<unsigned char> &data) = 0;

protected:
  /* Sécurité : vérifie qu'on ne lit pas hors du fichier */
  static bool _isOffsetSafe(const std::vector<unsigned char> &data,
                            size_t offset, size_t bytesNeeded);

  /* Charge le fichier en mémoire */
  static std::vector<unsigned char> _readFile(const std::string &filename);

  /* Lecture d'entiers avec gestion du byte order */
  static uint16_t _read16(const std::vector<unsigned char> &data, size_t offset,
                          bool bigEndian);
  static uint32_t _read32(const std::vector<unsigned char> &data, size_t offset,
                          bool bigEndian);
};

#endif