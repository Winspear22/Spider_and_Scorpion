/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 15:46:40 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 20:47:47 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __HTML_PARSER_HPP
#define __HTML_PARSER_HPP

#include "includes.hpp"

class HtmlParser
{
	public:
		HtmlParser(void);
		HtmlParser(const HtmlParser &src);
		const HtmlParser &operator=(const HtmlParser &src);
		~HtmlParser(void);

		static std::vector<std::string> extractImagesFromHtml(const std::string &html);
		static std::vector<std::string> extractLinksFromHtml(const std::string &html);

		static bool						hasValidExtensionBasic(const std::string &url);
		static bool						hasValidExtensionParticulars(const std::string &url);
		static bool						isValidLink(const std::string &url);

	private:
};

#endif