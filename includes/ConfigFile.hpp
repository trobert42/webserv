/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:15 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 12:42:02 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
# define CONFIGFILE_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "ServerContext.hpp"
#include "Directive.hpp"
#include "directives.hpp"
#include "utils.hpp"

#define UNKNOWN_DIRECTIVES_ERR "unknown directive"
#define UNKNOWN_CONTEXT_ERR "unknown context block"
#define MISSING_COLON_ERR "missing ':' in directive declaration"
#define INVALID_LOCAT_SYNTAX_ERR "invalid location context syntax"
#define INVALID_SERV_SYNTAX_ERR "invalid server context syntax"
#define INVALID_LINE_ERR "invalid line"
#define BAD_DIRECTIVE_ERR "directive/context not in good block"
#define NO_SERV_ERR "no server block"

typedef std::vector<ServerContext> 					VectorServers;
typedef std::vector<ServerContext>::iterator		VectorServersIterator;
typedef std::vector<ServerContext>::const_iterator	VectorServersConstIterator;

class ConfigFile
{
	public:
		ConfigFile(void);
		~ConfigFile(void);
		ConfigFile(std::string &conf_name);
		ConfigFile(ConfigFile const &cpy);
		ConfigFile &operator=(ConfigFile const &rhs);

		void	parseInfo(void);

		/* getters */
		VectorServers	getServerContexts(std::string port);
		VectorServers	&getServersVector(void);

		/*check of duplicates ports*/
		bool	checkPortServers(void);

		/* display functions*/
		void	printInfoServers(void);

		/* exceptions' classes */
		class InvalidConfFileException : public std::exception
		{
			public:
				InvalidConfFileException(const std::string &message)
				{
					m_message = "in config file parsing: ";
					m_message += message;
				}
				virtual ~InvalidConfFileException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};


	private:
		std::string		m_conf_name;
		std::ifstream	m_conf_stream;
		size_t			m_nbr_serv_block;
		VectorServers   m_servers;

		/* server context parsing */
		bool			isBeginServContext(std::string line) const;
		bool			isEndServContext(std::string line) const;
		void			parseServContext(void);
		void			addInfoServContext(ServerContext &serv_block, std::string line);

		/* location context parsing */
		bool			isBeginLocatContext(ServerContext &serv_block, std::string line);
		bool			isEndLocatContext(std::string line) const;
		void			parseLocatContext(ServerContext &serv_block);
		void			addInfoLocatVector(ServerContext &serv_block, std::string line);


};


#endif // CONFIGFILE_HPP