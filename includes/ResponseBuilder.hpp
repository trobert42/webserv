/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/02 21:48:49 by agras             #+#    #+#             */
/*   Updated: 2023/08/07 11:04:13 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "../includes/httpDatas.hpp"
#include "../includes/HttpTables.hpp"
#include "../includes/headerFields.hpp"
#include "../includes/ServerContext.hpp"
#include "../includes/Response.hpp"

class ResponseBuilder
{
public:
	ResponseBuilder() : http_tables(HttpTables::getInstance()), response(new Response()) {}
	~ResponseBuilder() { delete response; }

	void	reset() { delete response; response = new Response(); }
	void	setProtocol(s_http_version version);
	void	setProtocol(std::string version);
	void	setStatus(e_status_code status);
	void	setBody(std::string content);
	void	setBodyFromFile(std::string path);
	void	addHeaderField(HeaderField *header);
	Response	harvestResponse();
	const Response*	peekResponse() const;

	e_status_code	getStatus(void);


private:
	HttpTables*	http_tables;
	Response*	response;
};

#endif // RESPONSEBUILDER_HPP