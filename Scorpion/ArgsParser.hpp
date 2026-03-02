/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgsParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 22:50:36 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:00:54 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __ARGSPARSER_HPP__
#define __ARGSPARSER_HPP__

#include "includes.hpp"

class ArgsParser
{
public:
	ArgsParser();
	~ArgsParser();
	const ArgsParser &operator=(const ArgsParser &other);
	ArgsParser(const ArgsParser &other);

	static std::vector<std::string> parseFiles(int argc, char **argv);

private:

};

#endif
