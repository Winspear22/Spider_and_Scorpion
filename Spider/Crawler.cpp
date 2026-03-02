/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Crawler.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 20:49:54 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 21:10:08 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Crawler.hpp"
#include "HtmlParser.hpp"
#include "ImageDownloader.hpp"
#include "UrlParser.hpp"
#include "UseCurl.hpp"

Crawler::Crawler(void)
{
	std::cout << "Constructeur de Crawler utilisé." << std::endl;
}

Crawler::Crawler(const Crawler &src)
{
	std::cout << "Constructeur de recopie Crawler utilisé." << std::endl;
	*this = src;
}

const Crawler &Crawler::operator=(const Crawler &src)
{
	std::cout << "Operator= de Crawler utilisé." << std::endl;
	if (this == &src)
		return *this;
	this->_visited = src._visited;
	return *this;
}

Crawler::~Crawler(void)
{
	std::cout << "Destructeur de Crawler utilisé." << std::endl;
}

/*
 * crawl() — Point d'entrée public
 * Appelé par Spider::run(). Initialise le set des URLs visitées,
 * calcule le host de base, puis lance la récursion.
 */
void Crawler::crawl(const std::string &url, int maxDepth, const std::string &downloadPath, bool isRecursive)
{
	this->_visited.clear();

	// Récupérer le host de l'URL de départ pour ne pas sortir du domaine
	UrlParser baseParser;
	baseParser.parseUrl(url);
	std::string baseHost = baseParser.getHost();

	if (isRecursive)
		this->_crawlRecursive(url, 0, maxDepth, downloadPath, baseHost);
	else
		this->_crawlRecursive(url, 0, 0, downloadPath, baseHost);
}

/*
 * _crawlRecursive() — Le coeur du Spider 🕷️
 *
 * Algorithme :
 * 1. Vérifier les conditions d'arrêt (profondeur, déjà visité)
 * 2. Marquer l'URL comme visitée
 * 3. Récupérer le HTML
 * 4. Extraire et télécharger les images
 * 5. Si on n'a pas atteint la profondeur max, extraire les liens
 *    et relancer _crawlRecursive sur chaque lien du même domaine
 */
void Crawler::_crawlRecursive(const std::string &url, int depth, int maxDepth, const std::string &downloadPath, const std::string &baseHost)
{
	// Condition d'arrêt 1 : profondeur dépassée
	if (depth > maxDepth)
		return;

	// Condition d'arrêt 2 : URL déjà visitée (protection anti-boucle)
	if (this->_visited.find(url) != this->_visited.end())
		return;

	// Marquer comme visitée
	this->_visited.insert(url);
	std::cout << "[depth=" << depth << "] Crawling: " << url << std::endl;

	// Étape 3 : Récupérer le HTML de la page
	UseCurl useCurl;
	std::string html = useCurl.getHtml(url);
	if (html.empty())
	{
		std::cerr << "Error: HTML vide pour " << url << std::endl;
		return;
	}

	// Étape 4 : Extraire les images et les télécharger
	std::vector<std::string> imagesUrls = HtmlParser::extractImagesFromHtml(html);
	std::cout << "  → " << imagesUrls.size() << " images trouvées." << std::endl;

	UrlParser parser;
	ImageDownloader downloader;
	parser.parseUrl(url);

	size_t i = 0;
	while (i < imagesUrls.size())
	{
		std::string fullUrl = parser.resolveUrl(imagesUrls[i]);
		if (!fullUrl.empty())
			downloader.downloadImage(fullUrl, downloadPath);
		i++;
	}

	// Étape 5 : Si on peut encore descendre, extraire les liens et récurser
	if (depth < maxDepth)
	{
		std::vector<std::string> linksUrls = HtmlParser::extractLinksFromHtml(html);
		std::cout << "  → " << linksUrls.size() << " liens trouvés." << std::endl;

		size_t j = 0;
		while (j < linksUrls.size())
		{
			std::string fullLinkUrl = parser.resolveUrl(linksUrls[j]);

			// Vérification du domaine : on reste sur le même site
			UrlParser linkParser;
			linkParser.parseUrl(fullLinkUrl);
			if (linkParser.getHost() == baseHost)
			{
				this->_crawlRecursive(fullLinkUrl, depth + 1, maxDepth, downloadPath,
								baseHost);
			}
			j++;
		}
	}
}
