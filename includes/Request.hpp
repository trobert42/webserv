/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agras <agras@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/10 15:15:36 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 23:07:00 by agras            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <sys/time.h>

#include "../includes/ServerContext.hpp"
#include "../includes/headerFields.hpp"
#include "../includes/Response.hpp"

enum e_req_state
{
	READING_HEADER,
	READING_BODY,
	READY,
	RESPONSE_READY,
};

class	Request
{
public:
	Request(void) :
				state(READING_HEADER),
				method(GET),
				chunk_size(0),
				context(),
				is_server_attributed(false)
	{
		version.major = 0;
		version.minor = 0;
		gettimeofday(&created_time, NULL);

	};

	Request(const Request& other)
	{
		*this = other;
	}

	~Request() {
		for (std::map<std::string, HeaderField*>::iterator it = header_fields.begin();
				it != header_fields.end(); ++it)
			delete it->second;
	}

	Request &operator=(Request const &rhs) {
		if (&rhs != this)
		{
			this->state = rhs.state;
			this->method = rhs.method;
			this->url = rhs.url;
			this->version = rhs.version;
		    for (std::map<std::string, HeaderField*>::const_iterator it = rhs.header_fields.begin(); it != rhs.header_fields.end(); ++it)
				this->header_fields[it->first] = it->second->clone();
			this->chunk = rhs.chunk;
			this->chunk_size = rhs.chunk_size;
			this->body = rhs.body;
			this->response = rhs.response;
			this->location = rhs.location;
			this->filename = rhs.filename;
			this->context = rhs.context;
			this->created_time = rhs.created_time;
			this->is_server_attributed = rhs.is_server_attributed;
		}
		return *this;
	}

	void	printRequestInfo(void)
	{
		std::cout << method << " " << url.path << " " << "HTTP/" << version.major << "." << version.minor << std::endl;
		if (header_fields.count("host") > 0)
			std::cout << "host: " << header_fields["host"]->getValue() << std::endl;
		if (header_fields.count("user-agent") > 0)
			std::cout << "user-agent: " << header_fields["user-agent"]->getValue() << std::endl;
		if (header_fields.count("content-length") > 0)
			std::cout << "content-length: " << header_fields["content-length"]->getValue() << std::endl;
		if (header_fields.count("content-type") > 0)
			std::cout << "content-type: " << header_fields["content-type"]->getValue() << std::endl;
	}

	class InvalidHttpRequest : public std::exception
	{
		public:
			InvalidHttpRequest(const std::string &msg, const e_status_code status) : msg(msg), status(status) {}
			~InvalidHttpRequest() throw () {}
			virtual const char*	what() const throw () { return msg.c_str(); }
			e_status_code	getStatus() const { return status; }

		private:
			std::string		msg;
			e_status_code	status;

	};

	double	getRequestAge();
	bool	isAttributedToAServer() { return is_server_attributed; }

	e_req_state		state;
	e_req_method	method;
	s_url			url;
	s_http_version	version;
	std::map<std::string, HeaderField*>	header_fields;
	std::vector<char>	chunk;
	size_t			chunk_size;
	std::vector<char>	body;
	Response		response;
	std::string		filename;
	ServerContext	context;
	std::string		location;
	bool			is_server_attributed;

	friend class HttpWorker;
	friend class Connection;

private:
	struct timeval 	created_time;
};

std::ostream	&operator<<(std::ostream &out, Request &rhs);


#endif // REQUEST_HPP