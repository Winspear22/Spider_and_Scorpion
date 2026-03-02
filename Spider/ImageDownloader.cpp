/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImageDownloader.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:49:31 by adnen             #+#    #+#             */
/*   Updated: 2026/03/03 00:45:27 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImageDownloader.hpp"

ImageDownloader::ImageDownloader(void) {
  std::cout << "Constructeur de ImageDownloader utilisé." << std::endl;
}

ImageDownloader::ImageDownloader(const ImageDownloader &src) {
  std::cout << "Constructeur de recopie ImageDownloader utilisé." << std::endl;
  *this = src;
}

const ImageDownloader &ImageDownloader::operator=(const ImageDownloader &src) {
  std::cout << "Operator= de ImageDownloader utilisé." << std::endl;
  if (this == &src)
    return *this;
  this->_url = src._url;
  return *this;
}

ImageDownloader::~ImageDownloader(void) {
  std::cout << "Destructeur de ImageDownloader utilisé." << std::endl;
}

bool ImageDownloader::downloadImage(const std::string &url,
                                    const std::string &outputDir) {
  if (url.empty())
    return FAILURE;

  std::string filename = _extractFilename(url);
  if (filename.empty())
    return FAILURE;

  // Protection 1 : Path Traversal → on refuse les noms de fichier suspects
  if (filename.find("..") != std::string::npos ||
      filename.find('/') != std::string::npos) {
    std::cerr << "Error: Nom de fichier suspect (path traversal) → " << filename
              << std::endl;
    return FAILURE;
  }

  // Protection 5 : Nom de fichier trop long (limite Linux = 255 caractères)
  if (filename.length() > 200) {
    std::cerr << "Error: Nom de fichier trop long → " << filename.substr(0, 50)
              << "..." << std::endl;
    return FAILURE;
  }

  std::string dir = outputDir;
  if (!dir.empty() && dir.back() != '/') {
    dir += "/";
  }
  std::string fullPath = dir + filename;

  // Protection 2 : On ne veut pas écraser un fichier déjà existant
  try {
    if (std::filesystem::exists(fullPath)) {
      std::cout << "Skip: " << filename << " (existe déjà)" << std::endl;
      return SUCCESS;
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return FAILURE;
  }

  // Protection 3 : Exception handling sur la création du dossier
  try {
    std::filesystem::create_directories(outputDir);
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error: Impossible de créer le dossier → " << e.what()
              << std::endl;
    return FAILURE;
  }

  std::ofstream file(fullPath, std::ios::binary);
  if (!file.is_open()) {
    std::cerr << "Error: Impossible d'ouvrir le fichier " << fullPath
              << std::endl;
    return FAILURE;
  }

  CURL *curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Error: Impossible de créer la session CURL" << std::endl;
    return FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeToFile);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);

  CURLcode res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);

  if (res != CURLE_OK) {
    std::cerr << "Error: " << curl_easy_strerror(res) << " → " << url
              << std::endl;
    return FAILURE;
  }
  std::cout << "Success: " << filename << std::endl;
  return SUCCESS;
}

size_t ImageDownloader::_writeToFile(void *ptr, size_t size, size_t nmemb,
                                     void *stream) {
  std::ofstream *file = static_cast<std::ofstream *>(stream);
  file->write(static_cast<char *>(ptr), size * nmemb);

  // Protection 4 : Si l'écriture a échoué (disque plein, etc.), on retourne 0
  // → curl comprend que ça a foiré et arrête le téléchargement proprement
  if (!file->good())
    return 0;
  return size * nmemb;
}

std::string ImageDownloader::_extractFilename(const std::string &url) {
  size_t pos = url.rfind('/');
  std::string name;

  if (pos != std::string::npos)
    name = url.substr(pos + 1);
  else
    name = url;

  size_t query = name.find('?');
  if (query != std::string::npos)
    name = name.substr(0, query);

  size_t anchor = name.find('#');
  if (anchor != std::string::npos)
    name = name.substr(0, anchor);

  return name;
}
