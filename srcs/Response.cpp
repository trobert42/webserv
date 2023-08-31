/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 20:30:31 by agras             #+#    #+#             */
/*   Updated: 2023/08/02 11:04:27 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>

#include "../includes/Response.hpp"

Response::Response() : status(Ok)
{
	version.major = 0;
	version.minor = 0;
}

Response::Response(const Response& other)
{
	*this = other;
}

Response::~Response()
{
	for (size_t i = 0; i < header.size(); ++i)
		delete header[i];
}

Response&	Response::operator=(const Response& rhs)
{
	if (this == &rhs)
		return *this;
	this->status = rhs.status;
	this->version = rhs.version;
	for (size_t i = 0; i < rhs.header.size(); ++i)
		this->header.push_back(rhs.header[i]->clone());
	this->body = rhs.body;
	return *this;
}

const HeaderField*	Response::getHeader(size_t index) const
{
	if (index < header.size())
		return header[index];
	throw std::out_of_range("Out of range while accessing headers of response.");
}