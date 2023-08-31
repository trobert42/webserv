/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTables.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:42:09 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 19:47:38 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "../includes/HttpTables.hpp"
#include "../includes/httpDatas.hpp"

HttpTables*	HttpTables::instance = NULL;

HttpTables::HttpTables()
{
	status_codes_table[Ok] = "OK";
	status_codes_table[NotFound] = "Not Found";
	status_codes_table[BadRequest] = "Bad Request";
	status_codes_table[NoContent] = "No Content";
	status_codes_table[Created] = "Created";
	status_codes_table[PayloadTooLarge] = "Payload too large";

	addMIMEType("text/html", "html", "");
	addMIMEType("text/css", "css", "");
	addMIMEType("image/jpeg", "jpg", "");
	addMIMEType("image/png", "png", "");
	addMIMEType("application/octet-stream", "", "");

	supported_cgi_table.insert("php");
	supported_cgi_table.insert("py");
}

HttpTables*	HttpTables::getInstance()
{
	if (instance == NULL)
		instance = new HttpTables();
	return instance;
}

const std::string	HttpTables::getStatusDescription(e_status_code code)
{
	std::map<e_status_code, std::string>::iterator it = status_codes_table.find(code);
	if (it != status_codes_table.end())
		return it->second;
	else
		return std::string();
}


const std::vector<std::string> HttpTables::getMIMETypeExtensions(std::string MIME_type)
{
	std::map<std::string, s_mimetype>::iterator it = MIME_types_table.find(MIME_type);
	if (it != MIME_types_table.end())
		return it->second.extensions;
	else
		return std::vector<std::string>();
}

const std::string	HttpTables::getMIMEtype(std::string extension)
{
	for (std::map<std::string, s_mimetype>::iterator it = MIME_types_table.begin(); it != MIME_types_table.end(); it++)
	{
		if (std::find(it->second.extensions.begin(), it->second.extensions.end(), extension) != it->second.extensions.end())
			return it->first; 
	}
	return "application/octet-stream";
}

void	HttpTables::addMIMEType(std::string MIME_type, std::string extension, std::string path_to_script)
{
	MIME_types_table[MIME_type].extensions.push_back(extension);
	MIME_types_table[MIME_type].path_to_script = path_to_script;
}

bool	HttpTables::isCGIsupported(std::string extension)
{
	if (supported_cgi_table.count(extension) > 0)
		return true;
	return false;
}