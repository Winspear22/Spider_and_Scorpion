/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExifParserJPEG.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 00:29:00 by adnen             #+#    #+#             */
/*   Updated: 2026/04/05 11:45:16 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __EXIFPARSERJPEG_HPP__
#define __EXIFPARSERJPEG_HPP__

#include "ExifParser.hpp"

class ExifParserJPEG : public ExifParser
{
	public:
		ExifParserJPEG();
		~ExifParserJPEG();
		ExifParserJPEG(const ExifParserJPEG &other);
		const ExifParserJPEG &operator=(const ExifParserJPEG &other);

		void parse(const std::vector<unsigned char> &data);

	private:
		/* Trouve le segment APP1 contenant l'EXIF */
		int _findExifOffset(const std::vector<unsigned char> &data);

		/* Détecte "MM" (big) ou "II" (little) */
		bool _detectByteOrder(const std::vector<unsigned char> &data, size_t offset);

		/* Lit un tag ASCII depuis un IFD */
		std::string _readAsciiTag(const std::vector<unsigned char> &data, size_t tiffStart, size_t entryValueOffset, uint32_t count, bool bigEndian);

		/* Parcourt un IFD et affiche les tags connus */
		void _parseIFD(const std::vector<unsigned char> &data, size_t tiffStart, size_t ifdOffset, bool bigEndian, bool isExifSubIFD);
};

#endif
