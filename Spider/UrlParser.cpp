/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UrlParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:28:08 by adnen             #+#    #+#             */
/*   Updated: 2026/02/26 17:45:32 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UrlParser.hpp"
#include <cstddef>
#include <string>

UrlParser::UrlParser(void)
{
	  std::cout << "Constructeur de UrlParser utilisé." << std::endl;
}

UrlParser::UrlParser(const UrlParser &src)
{
	  std::cout << "Constructeur de copie de UrlParser utilisé." << std::endl;
  *this = src;
}

const UrlParser &UrlParser::operator=(const UrlParser &src)
{
	  std::cout << "Operator= de UrlParser utilisé." << std::endl;
  if (this == &src)
    return *this;
  this->_url = src._url;
  this->_protocol = src._protocol;
  this->_host = src._host;
  this->_basePath = src._basePath;
  return *this;
}

UrlParser::~UrlParser(void)
{
	  std::cout << "Destructeur de UrlParser utilisé." << std::endl;
}

std::string UrlParser::getUrl()
{
	return this->_url;
}

std::string UrlParser::getProtocol()
{
	return this->_protocol;
}

std::string UrlParser::getHost()
{
	return this->_host;
}

std::string UrlParser::getBasePath()
{
	return this->_basePath;
}

void UrlParser::setUrl(std::string src)
{
	this->_url = src;
}

void UrlParser::setProtocol(std::string src)
{
	this->_protocol = src;
}

void UrlParser::setHost(std::string src)
{
	this->_host = src;
}

void UrlParser::setBasePath(std::string src)
{
	this->_basePath = src;
}

void UrlParser::parseUrl(std::string src)
{
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

std::string UrlParser::resolveUrl(std::string src)
{
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

std::string UrlParser::_sanitizeUrl(std::string url)
{
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

	while ((pos = url.find("/../")) != std::string::npos)
	{
		prev = url.rfind('/', pos - 1);
		if (prev != std::string::npos && prev > 7)
			url.erase(prev, pos - prev + 3);
		else
			break;
	}
	return url;
}
