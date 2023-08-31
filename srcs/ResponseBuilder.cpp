/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agras <agras@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 21:53:11 by agras             #+#    #+#             */
/*   Updated: 2023/08/03 00:26:25 by agras            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <fstream>

#include "../includes/ResponseBuilder.hpp"
#include "../includes/httpDatas.hpp"
#include "../includes/HttpTables.hpp"

void	ResponseBuilder::setProtocol(s_http_version version)
{
	response->version = version;
}

void	ResponseBuilder::setProtocol(std::string version_str)
{
	std::stringstream	stream;
	s_http_version		version;

	stream << version_str;
	stream >> version;
	response->version = version;
}

void	ResponseBuilder::setStatus(e_status_code status)
{
	response->status = status;
}

e_status_code	ResponseBuilder::getStatus(void)
{
	return response->status;
}

// if content is read from a file, use setBody(string, string)
void	ResponseBuilder::setBody(std::string content)
{
	response->body = content;
	addHeaderField(new ContentLength(response->body.size()));
}

void	ResponseBuilder::setBodyFromFile(std::string path)
{
	std::ifstream file(path.c_str());
	std::stringstream buffer;
	if (file.is_open())
		buffer << file.rdbuf();
	response->body = buffer.str();
	addHeaderField(new ContentLength(response->body.size()));
	addHeaderField(new ContentType(path));
}

void	ResponseBuilder::addHeaderField(HeaderField* header)
{
	for (size_t i = 0; i < response->header.size(); i++)
		if (response->header[i]->getName() == header->getName())
			response->header.erase(response->header.begin() + i);
	response->header.push_back(header);
}

Response	ResponseBuilder::harvestResponse()
{
	Response	tmp = *response;
	reset();
	return tmp;
}

const Response*	ResponseBuilder::peekResponse() const
{
	return response;
}
