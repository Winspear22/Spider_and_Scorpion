/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Crawler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 20:49:57 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 21:11:57 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CRAWLER_HPP__
#define __CRAWLER_HPP__

#include "includes.hpp"
#include <set>

class Crawler
{
	public:
		Crawler(void);
		Crawler(const Crawler &src);
		const Crawler &operator=(const Crawler &src);
		~Crawler(void);

		void crawl(const std::string &url, int maxDepth, const std::string &downloadPath, bool isRecursive);

	private:
		void _crawlRecursive(const std::string &url, int depth, int maxDepth, const std::string &downloadPath, const std::string &baseHost);
		
		std::set<std::string> _visited;
};

#endif