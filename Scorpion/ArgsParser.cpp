/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgsParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 22:50:39 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:06:12 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ArgsParser.hpp"

ArgsParser::ArgsParser()
{
	std::cout << "ArgsParser constructor called." << std::endl;
}

ArgsParser::~ArgsParser()
{
	std::cout << "ArgsParser destructor called." << std::endl;
}

const ArgsParser &ArgsParser::operator=(const ArgsParser &other)
{
	if (this != &other)
	{

	}
	std::cout << "ArgsParser copy assignment operator called." << std::endl;
	return *this;
}

ArgsParser::ArgsParser(const ArgsParser &other)
{
	*this = other;
	std::cout << "ArgsParser copy constructor called." << std::endl;
}

std::vector<std::string> ArgsParser::parseFiles(int argc, char **argv)
{
    std::vector<std::string> validFiles;
    std::cout << "Parsing arguments..." << std::endl;
    int i;

    i = 1;
    while (i < argc)
    {
        std::filesystem::path filePath(argv[i]);
        if (!std::filesystem::exists(filePath))
            std::cerr << "Error: " << argv[i] << " not found." << std::endl;
        else
            validFiles.push_back(filePath.string());
        i++;
    }
    return validFiles;
}