/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/18 11:41:10 by trobert           #+#    #+#             */
/*   Updated: 2023/08/08 13:49:39 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

#include <sys/types.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include <fstream>
#include <map>

#include "httpDatas.hpp"
#include "ServerContext.hpp"
#include "Request.hpp"
#include "utils.hpp"

#define FORK_ERR "function fork() failed"
#define PIPE_ERR "function pipe() failed"
#define DUP2_ERR "function dup2() failed"
#define DUP_ERR "function dup() failed"
#define EXECVE_ERR "function execve() failed"
#define WRITE_ERR "function write() failed"
#define READ_ERR "function read() failed"
#define TIMEOUT_ERR "timeout cgi script"


typedef std::map<std::string, std::string>					MapString;
typedef std::map<std::string, std::string>::iterator		MapStringIterator;
typedef std::map<std::string, std::string>::const_iterator	MapStringConstIterator;

class CGIHandler
{
	public:
		CGIHandler(Request& request, std::string path, std::string location);
		CGIHandler(CGIHandler const & cpy);
		CGIHandler const &operator=(CGIHandler const & rhs);
		~CGIHandler(void);
		std::string	getBodyCGI(void);

		void		executeCGI(void);

		class ExecutionCGIFailedException : public std::exception
		{
			public:
				ExecutionCGIFailedException(const std::string &message)
				{
					m_message = "in cgi execution: ";
					m_message += message;
				}
				virtual ~ExecutionCGIFailedException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};

	private:
		CGIHandler(void);
		void		initEnv(void);
		char		**getEnv(void);
		std::string	getBin(void);

		Request				m_request;
		std::string			m_path;
		MapString			m_map_env;
		char				**m_env;
		std::vector<char>	m_body_request;
		std::string			m_body_response;
		size_t				m_body_size;
		std::string			m_location;
		char				*m_args[3];
};

#endif