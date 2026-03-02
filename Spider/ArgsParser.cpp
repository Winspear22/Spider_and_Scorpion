/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgsParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 23:03:02 by adnen             #+#    #+#             */
/*   Updated: 2026/02/22 17:18:33 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ArgsParser.hpp"
#include "includes.hpp"

ArgsParser::ArgsParser(void)
{
    std::cout << "Constructeur de ArgsParser utilisé." << std::endl;
}

ArgsParser::ArgsParser(const ArgsParser &src)
{
    std::cout << "Constructeur de recopie ArgsParser utilisé." << std::endl;
	*this = src;
}

const ArgsParser &ArgsParser::operator=(const ArgsParser &src)
{
	std::cout << "Operator= de ArgsParser utilisé." << std::endl;
	if (this == &src)
		return *this;
	return *this;
}

ArgsParser::~ArgsParser(void)
{
	std::cout << "Destructeur de ArgsParser utilisé." << std::endl;
}

bool ArgsParser::parseArguments(int argc, char **argv, Spider &spider)
{
	int opt;

	while ((opt = getopt(argc, argv, "rl:p:")) != -1)
	{
		switch (opt)
		{
			case 'r':
				spider.setIsRecursive(true);
				break;
			case 'l':
				try
				{
					spider.setDepthNumber(std::stoi(optarg));
				}
				catch (const std::invalid_argument &e)
				{
					std::cerr << "Error: Depth '-l' must be a valid number (e.g. -l 5)." << std::endl;
					return (FAILURE);
				}
				catch (const std::out_of_range &e)
				{
					std::cerr << "Error: Depth number is too large." << std::endl;
					return (FAILURE);
				}
				break;
			case 'p':
				spider.setPathOfDownload(optarg);
				break;
			case '?':
				std::cerr << "Error, wrong set of options." << std::endl;
				return FAILURE;
		}
	}
	if (optind < argc)
		spider.setUrl(argv[optind]);
	else
	{
		std::cerr << "Error, Missing URL." << std::endl;
		return FAILURE;
	}
	return SUCCESS;
}
