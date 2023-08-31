/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   httpDatas.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agras <agras@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/03 16:59:06 by agras             #+#    #+#             */
/*   Updated: 2023/08/01 21:41:28 by agras            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPDATAS_HPP
#define HTTPDATAS_HPP

#include <string>
#include <vector>
#include <map>

#include "../includes/header.hpp"

enum	e_status_code
{
	Continue = 100,
	SwitchingProtocol = 101,
	Processing = 102,
	EarlyHints = 103,

	Ok = 200,
	Created = 201,
	Accepted = 202,
	NonAuthoritativeInformation = 203,
	NoContent = 204,
	ResetContent = 205,
	PartialContent = 206,
	MultiStatus = 207,
	AlreadyReported = 208,
	ImUsed = 226,

	MultipleChoice = 300,
	MovedPermanently = 301,
	Found = 302,
	SeeOther = 303,
	NotModified = 304,
	UseProxy = 305,
	TemporaryRedirect = 307,
	PermanentRedirect = 308,

	BadRequest = 400,
	Unauthorized = 401,
	PaymentRequired = 402,
	Forbidden = 403,
	NotFound = 404,
	MethodNotAllowed = 405,
	NotAcceptable = 406,
	ProxyAuthenticationRequired = 407,
	RequestTimeout = 408,
	Conflict = 409,
	Gone = 410,
	LengthRequired = 411,
	PreconditionFailed = 412,
	PayloadTooLarge = 413,
	UriTooLong = 414,
	UnsupportedMediaType = 415,
	RangeNotSatisfiable = 416,
	ExpectationFailed = 417,
	MisdirectedRequest = 421,
	UnprocessableEntity = 422,
	Locked = 423,
	FailedDependency = 424,
	TooEarly = 425,
	UpgradeRequired = 426,
	PreconditionRequired = 428,
	TooManyRequests = 429,
	RequestHeaderFieldsTooLarge = 431,
	UnavailableForLegalReasons = 451,

	InternalServerError = 500,
	NotImplemented = 501,
	BadGateway = 502,
	ServiceUnavailable = 503,
	GatewayTimeout = 504,
	HttpVersionNotSupported = 505,
	VariantAlsoNegotiates = 506,
	InsufficientStorage = 507,
	LoopDetected = 508,
	NotExtended = 510,
	NetworkAuthenticationRequired = 511
};

enum e_req_method
{
	GET,
	POST,
	DELETE
};


std::ostream	&operator<<(std::ostream &out, const e_req_method &rhs);
std::istream	&operator>>(std::istream &in, e_req_method &rhs);
std::string		stringify_method(const e_req_method &rhs);

struct s_http_version
{
	uint16_t	major;
	uint16_t	minor;
};

std::ostream	&operator<<(std::ostream &out, const s_http_version &rhs);
std::istream	&operator>>(std::istream &in, s_http_version &rhs);

struct	s_uri
{
};

struct	s_url : public s_uri
{
	std::string	scheme;
	std::string	authority;
	std::string	port;
	std::string	path;
	std::string	query;
};

std::ostream	&operator<<(std::ostream &out, const s_url &rhs);

struct s_ReqBuffer
{
	std::vector<char>	data;
	size_t				head_pos;
};

struct s_mimetype
{
	std::vector<std::string>	extensions;
	std::string	path_to_script;
};

#endif // HTTPDATAS_HPP