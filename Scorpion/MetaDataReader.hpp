/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MetaDataReader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 23:52:31 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:52:34 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __METADATAREADER_HPP__
#define __METADATAREADER_HPP__

#include "includes.hpp"

class MetaDataReader
{
public:
	MetaDataReader();
	~MetaDataReader();
	const MetaDataReader &operator=(const MetaDataReader &other);
	MetaDataReader(const MetaDataReader &other);

private:

};

#endif
