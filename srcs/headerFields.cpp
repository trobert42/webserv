/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerFields.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 17:45:43 by agras             #+#    #+#             */
/*   Updated: 2023/08/07 11:12:02 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cstdio>

#include "../includes/headerFields.hpp"
#include "../includes/utils.hpp"
#include "../includes/httpDatas.hpp"
#include "../includes/HttpTables.hpp"

ContentLength::ContentLength(std::size_t length) : HeaderField("content-length"), length(length)
{
}

ContentLength::ContentLength(std::string value) : HeaderField("content-length"), length(atoi(value.c_str()))
{
}


ContentType::ContentType(std::string filename) : HeaderField("content-Type"), http_tables(HttpTables::getInstance())
{
	std::string extension = extractExtension(filename);

	MIME_type = http_tables->getMIMEtype(extension);
	encoding = "";
	boundary = "";
}

std::string	ContentType::getValue() const
{
	std::string ret = MIME_type;

	if (!encoding.empty())
		ret += "; charset=" + encoding;
	if (!boundary.empty())
		ret += "; boundary=" + boundary;
	return ret;
}