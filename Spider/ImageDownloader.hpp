/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImageDownloader.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:49:38 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 18:07:07 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __IMAGE_DOWNLOADER_HPP__
#define __IMAGE_DOWNLOADER_HPP__

#include "includes.hpp"
#include <filesystem>
#include <fstream>

class ImageDownloader {
public:
  ImageDownloader(void);
  ImageDownloader(const ImageDownloader &src);
  const ImageDownloader &operator=(const ImageDownloader &src);
  ~ImageDownloader(void);

  bool downloadImage(const std::string &url, const std::string &outputDir);

private:
  static size_t _writeToFile(void *ptr, size_t size, size_t nmemb,
                             void *stream);
  static std::string _extractFilename(const std::string &url);

  std::string _url;
};

#endif
