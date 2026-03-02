/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adnen <adnen@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:59:44 by adnen             #+#    #+#             */
/*   Updated: 2026/03/03 00:14:30 by adnen            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ArgsParser.hpp"
#include "ExifParser.hpp"
#include "FileValidator.hpp"
#include "MetaDataReader.hpp"
#include "includes.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: ./scorpion FILE1 [FILE2 ...]" << std::endl;
    return EXIT_FAILURE;
  }
  std::vector<std::string> files = ArgsParser::parseFiles(argc, argv);
  if (files.empty()) {
    std::cerr << "Error: no valid files provided." << std::endl;
    return EXIT_FAILURE;
  }

  int i = 0;
  while (i < (int)files.size()) {
    if (FileValidator::isFileOfValidFormat(files[i])) {
      MetaDataReader::displayMetadata(files[i]);
      ExifParser::displayExifData(files[i]);
    }
    i++;
  }
  return EXIT_SUCCESS;
}