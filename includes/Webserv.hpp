/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:41 by trobert           #+#    #+#             */
/*   Updated: 2023/08/02 19:32:54 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdlib.h>
#include <cerrno>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <algorithm>

#include "Server.hpp"
#include "ConfigFile.hpp"

class Webserv
{
	public:
		Webserv(char *av);
		~Webserv(void);
		Webserv(Webserv const &cpy);
		Webserv const &operator=(Webserv const &rhs);

		/* launching the server */
		void	run(void);

		/* exceptions' classes */
		class InvalidDefaultConfFileException : public std::exception
		{
			public:
				virtual const char* what() const throw()
				{
					return ("can't open or didn't find the default configuration file");
				}
		};

		class FailedConnectionServerException : public std::exception
		{
			public:
				FailedConnectionServerException(const std::string &message) { m_message = message; }
				virtual ~FailedConnectionServerException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};

	private:
		std::string	m_conf_name;
		ConfigFile	m_conf_file;

		Webserv(void);

		/* check and parse av[1] for config file */
		bool	isValidConfFile(void);
		void	getDefaultConfFile(void);
};

#endif // WEBSERV_HPP