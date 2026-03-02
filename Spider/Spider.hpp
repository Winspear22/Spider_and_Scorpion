/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Spider.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 20:12:43 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 21:06:07 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SPIDER_HPP__
#define __SPIDER_HPP__

#include "includes.hpp"

class Spider
{
	public:
		Spider(void);
		Spider(const Spider &src);
		const Spider &operator=(const Spider &src);
		~Spider(void);

		void run(void);

		bool getIsRecursive(void);
		void setIsRecursive(bool isRecursive);
		int getDepthNumber(void);
		void setDepthNumber(int nb);
		std::string getPathOfDownload(void);
		void setPathOfDownload(std::string path);
		std::string getUrl(void);
		void setUrl(std::string url);

	private:
		bool _isRecursive;
		int _depthNumber;
		std::string _pathOfDownload;
		std::string _url;
};

#endif