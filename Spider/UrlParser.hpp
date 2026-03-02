/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UrlParser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:28:11 by adnen             #+#    #+#             */
/*   Updated: 2026/02/26 17:46:17 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __URL_PARSER_HPP__
#define __URL_PARSER_HPP__

#include "includes.hpp"

class UrlParser
{
	public:
		UrlParser(void);
		UrlParser(const UrlParser &src);
		const UrlParser &operator=(const UrlParser &src);
		~UrlParser(void);

		std::string getUrl();
		std::string getProtocol();
		std::string getHost();
		std::string getBasePath();

		void setUrl(std::string src);
		void setProtocol(std::string src);
		void setHost(std::string src);
		void setBasePath(std::string src);

		void parseUrl(std::string src);
		std::string resolveUrl(std::string src);

	private:
		std::string _url;
		std::string _protocol;
		std::string _host;
		std::string _basePath;

		std::string _sanitizeUrl(std::string url);
};

#endif
