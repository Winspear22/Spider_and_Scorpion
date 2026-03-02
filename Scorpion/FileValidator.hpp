/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileValidator.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 23:10:04 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 23:10:08 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __FILEVALIDATOR_HPP__
#define __FILEVALIDATOR_HPP__

#include "includes.hpp"

class FileValidator
{
public:
	FileValidator();
	~FileValidator();
	const FileValidator &operator=(const FileValidator &other);
	FileValidator(const FileValidator &other);

private:

};

#endif
