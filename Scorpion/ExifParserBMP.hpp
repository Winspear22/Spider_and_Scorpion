/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserBMP.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 12:14:47 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXIFPARSERBMP_HPP__
#define __EXIFPARSERBMP_HPP__

#include "ExifParser.hpp"

class ExifParserBMP : public ExifParser
{
	public:
		ExifParserBMP();
		~ExifParserBMP();
		ExifParserBMP(const ExifParserBMP &other);
		const ExifParserBMP &operator=(const ExifParserBMP &other);

		void parse(const std::vector<unsigned char> &data);
};

#endif
