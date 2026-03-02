/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 23:10:01 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:10:22 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FileValidator.hpp"

FileValidator::FileValidator()
{
	std::cout << "FileValidator constructor called." << std::endl;
}

FileValidator::~FileValidator()
{
	std::cout << "FileValidator destructor called." << std::endl;
}

const FileValidator &FileValidator::operator=(const FileValidator &other)
{
	if (this != &other)
	{

	}
	std::cout << "FileValidator copy assignment operator called." << std::endl;
	return *this;
}

FileValidator::FileValidator(const FileValidator &other)
{
	*this = other;
	std::cout << "FileValidator copy constructor called." << std::endl;
}
