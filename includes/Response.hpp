/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/01 20:10:41 by agras             #+#    #+#             */
/*   Updated: 2023/08/02 11:04:58 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>

#include "../includes/httpDatas.hpp"
#include "../includes/headerFields.hpp"

class Response
{
public:
	Response();
	Response(const Response& other);
	~Response();

	Response&	operator=(const Response& rhs);

	s_http_version	getVersion() const { return version; }
	e_status_code		getStatus() const { return status; }
	const HeaderField*		getHeader(size_t index) const;
	const std::string&		getBody() const { return body; }
	size_t					getHeadersCount() { return header.size(); }

	friend class ResponseBuilder;

private:
	s_http_version				version;
	e_status_code				status;
	std::vector<HeaderField*>	header;
	std::string					body;
};


#endif // RESPONSE_HPP