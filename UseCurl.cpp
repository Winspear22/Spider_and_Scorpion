/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UseCurl.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 21:59:44 by adnen             #+#    #+#             */
/*   Updated: 2026/03/02 21:48:00 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UseCurl.hpp"

UseCurl::UseCurl(void)
{
	std::cout << "Constructeur de UseCurl utilisé." << std::endl;
}

UseCurl::UseCurl(const UseCurl &src)
{
	std::cout << "Constructeur de recopie UseCurl utilisé." << std::endl;
	*this = src;
}

const UseCurl &UseCurl::operator=(const UseCurl &src)
{
	std::cout << "Operator= de UseCurl utilisé." << std::endl;
	if (this == &src)
		return *this;
	return *this;
}

UseCurl::~UseCurl(void)
{
	std::cout << "Destructeur de UseCurl utilisé." << std::endl;
}

std::string UseCurl::getHtml(const std::string &url)
{
	std::string buffer;
	CURL *curl = curl_easy_init(); // Création de la session CURL en mémoire
	
	if (curl)
	{
		CURLcode result; // Variable de type CURLcode qui va stocker le résultat de la fonction curl_easy_perform
    	curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Le site où on doit se rendre
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeCallback); // La fonction que CURL doit utiliser (il ne doit pas gérer ça seul)
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer); // Le buffer dans lequel curl devra mettre tout ce qu'il télécharge
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Gestion des redirection (utile pour le http --> https)
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Gestion du timeout mis à 10 secondes
		curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT); // Curl ne se fait pas passer pour curl, mais pour un navigateur (certains sites refusent curl)
		result = curl_easy_perform(curl); // C'est ici que le téléchargement se fait
		if (result != CURLE_OK)
			std::cerr << "Error: Curl failed to fetch URL (" << url << ") - " << curl_easy_strerror(result) << std::endl;
		curl_easy_cleanup(curl); // Libération de la mémoire
	}
	else
		std::cerr << "Error: Impossible de créer la session CURL" << std::endl;
	return buffer;
}

size_t UseCurl::_writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize;
	std::string *buffer;

	realsize = size * nmemb;
	buffer = static_cast<std::string *>(userp);
	buffer->append(static_cast<char *>(contents), realsize);
	return realsize;
}
