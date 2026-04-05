/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 20:12:40 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:06:59 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.hpp"
#include "Crawler.hpp"

Spider::Spider(void)
{
	std::cout << "Constructeur de Spider utilisé." << std::endl;
	this->_depthNumber = 5;
	this->_isRecursive = false;
	this->_pathOfDownload = "./data/";
}

Spider::Spider(const Spider &src)
{
	std::cout << "Constructeur de recopie Spider utilisé." << std::endl;
	*this = src;
}

const Spider &Spider::operator=(const Spider &src)
{
	std::cout << "Operator= de Spider utilisé." << std::endl;
	if (this == &src)
		return *this;
	this->_depthNumber = src._depthNumber;
	this->_isRecursive = src._isRecursive;
	this->_pathOfDownload = src._pathOfDownload;
	this->_url = src._url;
	return *this;
}

Spider::~Spider(void)
{
	std::cout << "Destructeur de Spider utilisé." << std::endl;
}

bool Spider::getIsRecursive(void)
{
	return this->_isRecursive;
}

void Spider::setIsRecursive(bool isRecursive)
{
	this->_isRecursive = isRecursive;
}

int Spider::getDepthNumber(void)
{
	return this->_depthNumber;
}

void Spider::setDepthNumber(int nb)
{
  if (nb < 0)
    nb = 0;
  this->_depthNumber = nb;
}

std::string Spider::getPathOfDownload(void)
{
	return this->_pathOfDownload;
}

void Spider::setPathOfDownload(std::string path)
{
	this->_pathOfDownload = path;
}

std::string Spider::getUrl(void)
{
	return this->_url;
}

void Spider::setUrl(std::string url)
{
	this->_url = url;
}

void Spider::run(void)
{
	Crawler crawler;
	crawler.crawl(this->_url, this->_depthNumber, this->_pathOfDownload, this->_isRecursive);
}