/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImageDownloader.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/26 17:49:31 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 17:56:41 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImageDownloader.hpp"
#include "includes.hpp"
#include <cstdio>

ImageDownloader::ImageDownloader(void)
{
	std::cout << "Constructeur de ImageDownloader utilisé." << std::endl;
}

ImageDownloader::ImageDownloader(const ImageDownloader &src)
{
	std::cout << "Constructeur de recopie ImageDownloader utilisé." << std::endl;
	*this = src;
}

const ImageDownloader &ImageDownloader::operator=(const ImageDownloader &src)
{
	std::cout << "Operator= de ImageDownloader utilisé." << std::endl;
	if (this == &src)
		return *this;
	this->_url = src._url;
	return *this;
}

ImageDownloader::~ImageDownloader(void)
{
	std::cout << "Destructeur de ImageDownloader utilisé." << std::endl;
}

bool ImageDownloader::downloadImage(const std::string &url, const std::string &outputDir)
{
	if (url.empty())
		return FAILURE;

	std::string filename = _extractFilename(url);
	if (filename.empty())
		return FAILURE;

	std::string fullPath = outputDir + filename;

	mkdir(outputDir.c_str(), 0755);

	FILE *fp = fopen(fullPath.c_str(), "wb");
	if (!fp)
	{
		std::cerr << "Error: Impossible d'ouvrir le fichier " << fullPath
				<< std::endl;
		return FAILURE;
	}

	CURL *curl = curl_easy_init();
	if (!curl)
	{
		fclose(fp);
		std::cerr << "Error: Impossible de créer la session CURL" << std::endl;
		return FAILURE;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeToFile);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);

	CURLcode res = curl_easy_perform(curl);

	fclose(fp);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{
		std::cerr << "Error: " << curl_easy_strerror(res) << " → " << url << std::endl;
		return FAILURE;
	}
	std::cout << "Success: " << filename << std::endl;
	return SUCCESS;
}

size_t ImageDownloader::_writeToFile(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}

std::string ImageDownloader::_extractFilename(const std::string &url)
{
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
