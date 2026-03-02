/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MetaDataReader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 23:52:28 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:52:37 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MetaDataReader.hpp"

MetaDataReader::MetaDataReader()
{
	std::cout << "MetaDataReader constructor called." << std::endl;
}

MetaDataReader::~MetaDataReader()
{
	std::cout << "MetaDataReader destructor called." << std::endl;
}

const MetaDataReader &MetaDataReader::operator=(const MetaDataReader &other)
{
	if (this != &other)
	{

	}
	std::cout << "MetaDataReader copy assignment operator called." << std::endl;
	return *this;
}

MetaDataReader::MetaDataReader(const MetaDataReader &other)
{
	*this = other;
	std::cout << "MetaDataReader copy constructor called." << std::endl;
}
