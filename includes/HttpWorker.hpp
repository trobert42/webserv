/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpWorker.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/20 19:35:46 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 13:47:12 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <dirent.h>

#include "../includes/header.hpp"
#include "../includes/httpDatas.hpp"
#include "../includes/ResponseBuilder.hpp"
#include "../includes/HttpTables.hpp"
#include "../includes/headerFields.hpp"
#include "../includes/CGIHandler.hpp"
#include "../includes/ConfigFile.hpp"
#include "../includes/Connection.hpp"
#include "../includes/Request.hpp"

class HttpWorker
{
public:
	HttpWorker();
	~HttpWorker();

	void		handleRequest(Connection& connection, std::string ip);
	void		parseRequest(Request& request, Connection& connection);
	void		retrieveRequestBody(Request& request, Connection& connetion);
	void		serveRequest(Request& request);
	void		writeResponseToBuffer(Connection& connection, Response& response);

	int			checkErrorPageDirective(Request& request, e_status_code status, std::string& body);
	void		makeResponse(Request& request, e_status_code status, std::string body);
	void		makeResponseFromFile(e_status_code status, std::string path);
	bool		isCGI(std::string path);
	void		handlePostMultipart(Request &request);
	void		respondError(Connection& connection, Request request, const Request::InvalidHttpRequest& exception);
	int			retrieveChunk(Request& request, Connection& connection);


private:
	HttpTables*		http_tables;
	ResponseBuilder	response_builder;

	void		processGet(Request &request);
	void		processPost(Request &request);
	void		processDelete(Request &request);
	s_url		parseUrl(std::string str);
	std::string	decodeUrlEncodedChars(std::string encoded_str);
	void		parseStatusLine(Request& request, std::string line);
	bool		isSameHeader(std::string a, std::string b);
	bool		isMethodValid(Request& request) const;
	void		processResponse(Request& request, Connection& connection);
};

#endif // HTTP_SERVER_H