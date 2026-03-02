/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 15:46:37 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 20:40:57 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HtmlParser.hpp"
#include "includes.hpp"
#include <cstring>

HtmlParser::HtmlParser(void)
{
	std::cout << "Constructeur de HtmlParser utilisé." << std::endl;
}

HtmlParser::HtmlParser(const HtmlParser &src)
{
	std::cout << "Constructeur de recopie HtmlParser utilisé." << std::endl;
	*this = src;
}

const HtmlParser &HtmlParser::operator=(const HtmlParser &src)
{
	std::cout << "Operator= de HtmlParser utilisé." << std::endl;
	if (this == &src)
		return *this;
	return *this;
}

HtmlParser::~HtmlParser(void)
{
	std::cout << "Destructeur de HtmlParser utilisé." << std::endl;
}

/*
 * Regex breakdown :
 * <img       : Cherche le début de la balise (insensible à la casse via icase)
 * [^>]+      : [^>] veut dire "pas >", le + veut dire "au moins un". Saute les
 * autres attributs. src        : Cherche le mot "src" \s*=\s*    : \s* (espaces
 * facultatifs) suivis de = puis encore \s* (espaces facultatifs)
 * ["']       : Cherche soit un guillemet double " soit un simple '
 * ([^"']+)   : (...) = Capture ce qu'il y a dedans. [^"']+ = au moins un
 * caractère qui n'est ni " ni '
 * ["']       : Cherche le guillemet de fermeture correspond au début
 */
std::vector<std::string> HtmlParser::extractImagesFromHtml(const std::string &html)
{
	std::vector<std::string> urls;
	std::regex imgRegex(R"(<img[^>]+src\s*=\s*["']([^"']+)["'])", std::regex::icase);

	std::sregex_iterator it;
	std::sregex_iterator begin(html.begin(), html.end(), imgRegex);
	std::sregex_iterator end;

	it = begin;
	while (it != end)
	{
		std::smatch match = *it;
		std::string imgUrl = match[1].str();
		if (HtmlParser::hasValidExtensionParticulars(imgUrl))
			urls.push_back(imgUrl);
		it++;
	}
	return urls;
}

bool HtmlParser::hasValidExtensionBasic(const std::string &url)
{
	size_t pos;
	std::string image_extension;
	std::string tab_ext[] = {".jpeg", ".jpg", ".png", ".gif", ".bmp"};
	size_t i;

	pos = url.rfind('.');
	if (pos != std::string::npos)
	{
		image_extension = url.substr(pos);
		i = 0;
		while (i < image_extension.length())
		{
			image_extension[i] = std::tolower(image_extension[i]);
			i++;
		}
		if (image_extension == tab_ext[0])
			return SUCCESS;
		else if (image_extension == tab_ext[1])
			return SUCCESS;
		else if (image_extension == tab_ext[2])
			return SUCCESS;
		else if (image_extension == tab_ext[3])
			return SUCCESS;
		else if (image_extension == tab_ext[4])
			return SUCCESS;
		else
			return FAILURE;
	}
	return FAILURE;
}

bool HtmlParser::hasValidExtensionParticulars(const std::string &url)
{
	size_t end_pos;
	std::string clean_url;

	end_pos = url.find_first_of("?#");
	if (end_pos == std::string::npos)
		clean_url = url;
	else
		clean_url = url.substr(0, end_pos);
	return (HtmlParser::hasValidExtensionBasic(clean_url));
}

std::vector<std::string> HtmlParser::extractLinksFromHtml(const std::string &html)
{
	std::vector<std::string> urls;
	std::regex linkRegex(R"(<a[^>]+href\s*=\s*["']([^"'#]+)["'])", std::regex::icase);

	std::sregex_iterator it;
	std::sregex_iterator begin(html.begin(), html.end(), linkRegex);
	std::sregex_iterator end;

	it = begin;
	while (it != end)
	{
		std::smatch match = *it;
		std::string linkUrl = match[1].str();
		if (HtmlParser::isValidLink(linkUrl))
			urls.push_back(linkUrl);
		it++;
	}
	return urls;
}

bool HtmlParser::isValidLink(const std::string &url)
{
	if (url.empty())
		return FAILURE;
	if (url.find("mailto:") == 0)
		return FAILURE;
	if (url.find("javascript:") == 0)
		return FAILURE;
	if (url.find("tel:") == 0)
		return FAILURE;
	return SUCCESS;
}