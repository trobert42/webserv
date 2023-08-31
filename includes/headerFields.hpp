/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headerFields.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agras <agras@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 16:54:24 by agras             #+#    #+#             */
/*   Updated: 2023/07/30 03:28:13 by agras            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERFIELDS_HPP
#define HEADERFIELDS_HPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <cmath>
#include <sstream>

#include "../includes/header.hpp"
#include "../includes/HttpTables.hpp"

class ContentLength : public HeaderField
{
public:
	ContentLength(std::size_t length);
	ContentLength(std::string value);
	ContentLength(const ContentLength& other) : HeaderField(other), length(other.length) {}
	virtual ~ContentLength() {};

	virtual std::string getValue() const { return stringify(length); }
	virtual HeaderField*	clone() const { return new ContentLength(*this); }

private:
	std::size_t	length;
};

class ContentType : public HeaderField
{
public:
	ContentType(std::string filename);
	ContentType(const ContentType& other) : HeaderField(other),
											http_tables(other.http_tables->getInstance()),
											MIME_type(other.MIME_type), encoding(other.encoding),
											boundary(other.boundary) {}
	virtual ~ContentType() {};

	virtual std::string	getValue() const;
	virtual HeaderField*	clone() const { return new ContentType(*this); }

private:
	HttpTables*	http_tables;
	std::string MIME_type;
	std::string	encoding;
	std::string	boundary;
};

class TransferEncoding : public HeaderField
{
public:
	TransferEncoding(std::string encoding) : HeaderField("transfer-encoding"), encoding(encoding) {}
	TransferEncoding(const TransferEncoding& other) : HeaderField(other), encoding(other.encoding) {}
	virtual ~TransferEncoding() {}

	virtual std::string getValue() const { return encoding; }
	virtual HeaderField*	clone() const { return new TransferEncoding(*this); }

private:
	std::string encoding;
};

class Location : public HeaderField
{
public:
	Location(std::string new_path) : HeaderField("location"), path(new_path) {}
	Location(const Location& other) : HeaderField(other), path(other.path) {}
	virtual ~Location() {}

	virtual std::string getValue() const { return path; }
	virtual HeaderField*	clone() const { return new Location(*this); }

private:
	std::string	path;
};

class GenericHeader : public HeaderField
{
public:
	GenericHeader(std::string name, std::string value) : HeaderField(name), value(value) {}
	GenericHeader(const GenericHeader& other) : HeaderField(other), value(other.value) {}
	virtual ~GenericHeader() {}

	virtual std::string getValue() const { return value; }
	virtual HeaderField*	clone() const { return new GenericHeader(*this); }

private:
	std::string value;
};

#endif