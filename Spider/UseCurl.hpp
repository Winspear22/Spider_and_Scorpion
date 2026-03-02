/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UseCurl.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 21:59:45 by adnen             #+#    #+#             */
/*   Updated: 2026/02/21 22:17:04 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __USECURL_HPP__
#define __USECURL_HPP__

#include "includes.hpp"

class UseCurl
{
	public:
		UseCurl(void);
		UseCurl(const UseCurl &src);
		const UseCurl &operator=(const UseCurl &src);
		~UseCurl(void);
		
		std::string		getHtml(const std::string& url);
	private:
		static size_t	_writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

};

#endif