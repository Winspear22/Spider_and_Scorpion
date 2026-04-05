/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserGIF.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:13:13 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXIFPARSERGIF_HPP__
#define __EXIFPARSERGIF_HPP__

#include "ExifParser.hpp"

class ExifParserGIF : public ExifParser
{
	public:
		ExifParserGIF();
		~ExifParserGIF();
		ExifParserGIF(const ExifParserGIF &other);
		const ExifParserGIF &operator=(const ExifParserGIF &other);

		void parse(const std::vector<unsigned char> &data);
};

#endif
