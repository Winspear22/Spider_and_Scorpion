/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UrlParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:28:08 by adnen             #+#    #+#             */
/*   Updated: 2026/03/22 08:09:24 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UrlParser.hpp"
#include <cstddef>
#include <string>

UrlParser::UrlParser(void) {
  std::cout << "Constructeur de UrlParser utilisé." << std::endl;
}

UrlParser::UrlParser(const UrlParser &src) {
  std::cout << "Constructeur de copie de UrlParser utilisé." << std::endl;
  *this = src;
}

const UrlParser &UrlParser::operator=(const UrlParser &src) {
  std::cout << "Operator= de UrlParser utilisé." << std::endl;
  if (this == &src)
    return *this;
  this->_url = src._url;
  this->_protocol = src._protocol;
  this->_host = src._host;
  this->_basePath = src._basePath;
  return *this;
}

UrlParser::~UrlParser(void) {
  std::cout << "Destructeur de UrlParser utilisé." << std::endl;
}

std::string UrlParser::getUrl() { return this->_url; }

std::string UrlParser::getProtocol() { return this->_protocol; }

std::string UrlParser::getHost() { return this->_host; }

std::string UrlParser::getBasePath() { return this->_basePath; }

void UrlParser::setUrl(std::string src) { this->_url = src; }

void UrlParser::setProtocol(std::string src) { this->_protocol = src; }

void UrlParser::setHost(std::string src) { this->_host = src; }

void UrlParser::setBasePath(std::string src) { this->_basePath = src; }

/*
 * parseUrl() — L'analyseur d'adresse
 * Cette fonction "découpe" l'URL de base pour comprendre son contexte.
 *
 * Exemple : https://42.fr/campus/paris/index.html
 * - _protocol : Le moyen de transport (ex: https://)
 * - _host     : La racine du site (ex: https://42.fr)
 * - _basePath : Le dossier actuel pour les liens relatifs (ex:
 * https://42.fr/campus/paris/)
 */

void UrlParser::parseUrl(std::string src) {
  size_t pos;
  size_t pos2;
  size_t pos3;
  this->_url = src;

  pos = src.find("://");
  if (pos != std::string::npos)
    this->_protocol = src.substr(0, pos + 3);

  pos2 = src.find('/', pos + 3);
  if (pos2 != std::string::npos)
    this->_host = src.substr(0, pos2);
  else
    this->_host = src;

  pos3 = src.find_last_of('/');
  if (pos3 != std::string::npos && pos3 > 7)
    this->_basePath = src.substr(0, pos3 + 1);
  else
    this->_basePath = this->_host + "/";
}

/*
 * resolveUrl() — La boussole du Spider
 * Transforme les liens "paresseux" (relatifs) trouvés dans le HTML en adresses
 * complètes.
 *
 * Basé sur : https://42.fr/campus/paris/index.html
 * 1. Absolu             : http://google.com/img.jpg ->
 * http://google.com/img.jpg
 * 2. Protocole-Relatif  : //cdn.com/icon.png        -> https://cdn.com/icon.png
 * 3. Racine-Relatif     : /logo.png                 -> https://42.fr/logo.png
 * 4. Relatif pur        : photo.jpg                 ->
 * https://42.fr/campus/paris/photo.jpg
 * 5. Data (Base64)      : data:image/...            -> "" (Ignoré)
 */

std::string UrlParser::resolveUrl(std::string src) {
  if (src.find("data:") == 0)
    return "";
  if (src.find("http://") == 0 || src.find("https://") == 0 ||
      src.find("HTTP://") == 0 || src.find("HTTPS://") == 0)
    return this->_sanitizeUrl(src);
  else if (src.find("//") == 0)
    return this->_sanitizeUrl(this->_protocol + src.substr(2));
  else if (src.find('/') == 0)
    return this->_sanitizeUrl(this->_host + src);
  else
    return this->_sanitizeUrl(this->_basePath + src);
}

/*
 * _sanitizeUrl : "Le Nettoyeur d'URL" 🧹
 *
 * Imagine que cette fonction est un agent d'entretien qui passe après
 * que l'on ait construit une adresse, pour s'assurer qu'elle est "propre".
 *
 * 1. NETTOYAGE DES MAJUSCULES (La Casse) :
 *    Si quelqu'un écrit "HTTP://google.com", l'agent le transforme en
 *    "http://google.com". C'est la norme, et ça évite les erreurs plus tard.
 *
 * 2. NETTOYAGE DES POINTS INUTILES (Path : /./) :
 *    Le symbole "." veut dire "ici".
 *    "42.fr/./campus" est une manière compliquée de dire "42.fr/campus".
 *    L'agent supprime donc tous les "/./".
 *
 * 3. CALCUL DES REMONTÉES (Path : /../) :
 *    Le symbole ".." veut dire "reculer d'un dossier".
 *    Si on a "mondossier/sousdossier/../image.jpg", l'agent comprend
 *    que l'on entre dans "sousdossier" pour en ressortir aussitôt.
 *    Il simplifie donc l'adresse en : "mondossier/image.jpg".
 *
 * 4. SÉCURITÉ DU PROTOCOLE :
 *    L'agent s'arrête de "reculer" s'il arrive au niveau du "http://",
 *    pour éviter d'effacer le début de l'adresse par erreur.
 *
 * RÉSULTAT : On obtient une URL standardisée, sans "bruit" inutile.
 */

std::string UrlParser::_sanitizeUrl(std::string url) {
  size_t pos;
  size_t prev;

  if (url.find("data:") == 0)
    return "";

  if (url.find("HTTP://") == 0)
    url.replace(0, 4, "http");
  else if (url.find("HTTPS://") == 0)
    url.replace(0, 5, "https");

  while ((pos = url.find("/./")) != std::string::npos)
    url.erase(pos, 2);

  while ((pos = url.find("/../")) != std::string::npos) {
    prev = url.rfind('/', pos - 1);
    if (prev != std::string::npos && prev > 7)
      url.erase(prev, pos - prev + 3);
    else
      break;
  }
  return url;
}
