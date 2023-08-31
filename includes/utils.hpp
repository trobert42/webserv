/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 17:35:33 by agras             #+#    #+#             */
/*   Updated: 2023/08/02 21:21:21 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <string.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

#define BLACK 	"\033[30m"
#define WHITE 	"\033[37m"
#define GREEN 	"\033[32m"
#define BLUE	"\033[34m"
#define RED		"\033[31m"
#define YELLOW	"\033[33m"
#define MAGENTA	"\033[35m"
#define CYAN	"\033[36m"
#define GREY	"\033[90m"

typedef std::vector<std::string> VectorString;
typedef std::vector<std::string>::iterator VectorIterator;
typedef std::vector<std::string>::const_iterator VectorConstIterator;

void			removeChar(std::string &line, const char *charsToRemove);
VectorString	splitVector(std::string line, char c);
VectorString	splitVector2(std::string line, std::string chars);
void			printInfoVector(VectorString vector);
bool			isEmpty(std::string line);
std::string		vectorCharToString(std::vector<char> vec);

template<typename T>
std::string stringify(const T n)
{
	std::stringstream	stream;
	std::string			s;

	stream << n;
	stream >> s;
	return s;
}

std::string	extractExtension(const std::string filename);

std::string trimFrontSpaces(std::string str);

std::vector<char>::iterator	findRangeinRange(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::vector<char>::iterator to_find_begin, std::vector<char>::iterator to_find_end);
std::vector<char>::iterator	findRangeinRangeLimit(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::vector<char>::iterator to_find_begin, std::vector<char>::iterator to_find_end, int max_bytes);
std::string	tolowerString(std::string s);

bool		areEqualRanges(std::vector<char> a, std::vector<char> b);
bool		isEqualToString(std::vector<char> vec, std::string str);
bool		isFileInaccessible(std::string filename);
bool		isFileExisting(std::string filename);
bool		isFileReadable(std::string filename);

#endif // UTILS_HPP
