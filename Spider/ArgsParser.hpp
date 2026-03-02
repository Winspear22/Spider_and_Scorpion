/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgsParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 23:03:05 by adnen             #+#    #+#             */
/*   Updated: 2026/02/22 15:45:20 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Spider.hpp"
#include "includes.hpp"

class ArgsParser
{
	public:
		ArgsParser(void);
		ArgsParser(const ArgsParser &src);
		const ArgsParser &operator=(const ArgsParser &src);
		~ArgsParser(void);

		static bool parseArguments(int argc, char **argv, Spider &spider);

	private:
};
