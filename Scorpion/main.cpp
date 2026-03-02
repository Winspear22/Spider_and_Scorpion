/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:59:44 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:01:58 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes.hpp"
#include "ArgsParser.hpp"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Error: Not enough arguments." << std::endl;
		return (EXIT_FAILURE);
	}
    std::vector<std::string> files = ArgsParser::parseFiles(argc, argv);
    if (files.empty())
    {
        std::cerr << "Error: no valid files provided." << std::endl;
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}