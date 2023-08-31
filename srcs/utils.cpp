/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 21:32:47 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 13:50:39 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "../includes/utils.hpp"
#include "../includes/httpDatas.hpp"
#include "../includes/Request.hpp"

void removeChar(std::string &line, const char *charsToRemove)
{
	for (std::string::iterator it = line.begin(); it != line.end();)
	{
		if (strchr(charsToRemove, *it) != NULL)
			it = line.erase(it);
		else
			++it;
	}
}

std::vector<std::string> splitVector(std::string line, char c)
{
	std::vector<std::string> splited;
	std::string word;

	for (size_t i = 0; i < line.size(); i++)
	{
		word.clear();
		while (i < line.size() && line[i] != c && line[i] != '\t')
		{
			word += line[i];
			i++;
		}
		if (!word.empty())
			splited.push_back(word);
	}
	return splited;
}

bool	isChars(char c, std::string chars)
{
	for (int i = 0; i < (int) chars.size(); i++)
	{
		if (chars[i] == c)
			return true;
	}
	return false;
}

std::vector<std::string> splitVector2(std::string line, std::string chars)
{
	std::vector<std::string> splited;
	std::string word;

	for (size_t i = 0; i < line.size(); i++)
	{
		word.clear();
		while (i < line.size() && !isChars(line[i], chars))
		{
			word += line[i];
			i++;
		}
		if (!word.empty())
			splited.push_back(word);
	}
	return splited;
}

bool	isEmpty(std::string line)
{
	std::string old_line(line);
	if (old_line.empty())
		return true;
	removeChar(old_line, " \t");
	if (old_line.empty())
		return true;
	return false;
}

void	printInfoVector(VectorString vector)
{
	VectorIterator it = vector.begin();
	for (; it != vector.end(); ++it)
		std::cout << "[" << *it << "] ";
	std::cout << std::endl;
}

std::string	extractExtension(const std::string filename)
{
	return (filename.substr(filename.find('.') + 1, std::string::npos));
}

std::string trimFrontSpaces(std::string str)
{
	size_t i = 0;
	for (; i < str.size() && str[i] == ' '; i++);
	return str.substr(i);
}

//	check if all elements included between to_find_begin and to_find_end (to_find_end excluded)
//	are present consecutively in the range of elements included between begin and end (end excluded)
//	if found : returns an iterator to the first element of the first match
//	if not found : returns end
std::vector<char>::iterator	findRangeinRange(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::vector<char>::iterator to_find_begin, std::vector<char>::iterator to_find_end)
{
	std::vector<char>::iterator	i = begin;
	std::vector<char>::iterator ret = end;

	while (i != end)
	{
		if (*i == *to_find_begin)
		{
			ret = i;
			std::vector<char>::iterator	j = to_find_begin;
			while (i != end && *i == *j)
			{
				i++;
				j++;
				if (j == to_find_end)
					return ret;
			}
		}
		i++;
	}
	return ret;
}

std::vector<char>::iterator	findRangeinRangeLimit(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::vector<char>::iterator to_find_begin, std::vector<char>::iterator to_find_end, int max_bytes)
{
	std::vector<char>::iterator	i = begin;
	std::vector<char>::iterator ret = begin;
	std::vector<char>::iterator	j = to_find_begin;
	int bytes_nb = 0;

	while (i != end)
	{
		if (*i == *to_find_begin && j == to_find_begin)
			ret = i;

		if (*i == *j)
			j++;
		else
			j = to_find_begin;

		if (j == to_find_end)
		{
			if (ret - begin != max_bytes - 2)
				throw Request::InvalidHttpRequest("bad request payload formatting, BAD REQUEST 400 ERROR", BadRequest);
			return ret;
		}
		i++;
		bytes_nb++;
		if (bytes_nb >= max_bytes)
			throw Request::InvalidHttpRequest("request payload chunk's length does not match, BAD REQUEST 400 ERROR", BadRequest);
	}
	return end;
}

bool	areEqualRanges(std::vector<char> a, std::vector<char> b)
{
	if (a.size() != b.size())
		return false;
	for (size_t i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

bool	isEqualToString(std::vector<char> vec, std::string str)
{
	if (vec.size() != str.size())
		return false;
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] != str[i])
			return false;
	}
	return true;
}

std::string	tolowerString(std::string s)
{
	for (size_t i = 0; i < s.size(); i++)
		s[i] = std::tolower(s[i]);
	return s;
}

std::string	vectorCharToString(std::vector<char> vec)
{
	std::string ret;
	std::vector<char>::iterator it = vec.begin();
	for (;it != vec.end(); ++it)
		ret += *it;
	return ret;
}

bool	isFileInaccessible(std::string filename)
{
	struct stat buf;

	if (stat(filename.c_str(), &buf) == 0)
	{
		if (buf.st_mode & S_IRUSR)
			return false;
		if (buf.st_mode & S_IWUSR)
			return false;
		if (buf.st_mode & S_IXUSR)
			return false;
		return true;
	}
	return false;
}

bool isFileExisting(std::string filename)
{
	if (access(filename.c_str(), F_OK) < 0)
		return false;
	return true;
}

bool isFileReadable(std::string filename)
{
	if (access(filename.c_str(), F_OK | R_OK) < 0)
		return false;
	return true;
}