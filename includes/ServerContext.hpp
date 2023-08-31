/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:36 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 12:46:15 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONTEXT_HPP
# define SERVERCONTEXT_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <arpa/inet.h>

#include "Directive.hpp"
#include "directives.hpp"
#include "utils.hpp"

typedef	std::vector<Directive> 					DirectiveVector;
typedef	std::vector<Directive>::iterator 		DirectiveIterator;
typedef	std::vector<Directive>::const_iterator	DirectiveConstIterator;

typedef	std::map<std::string, DirectiveVector> 					MapLocation;
typedef	std::map<std::string, DirectiveVector>::iterator 		MapLocationIterator;
typedef	std::map<std::string, DirectiveVector>::const_iterator	MapLocationConstIterator;

class ServerContext
{
	public:
		ServerContext(void);
		~ServerContext(void);
		ServerContext(ServerContext const &cpy);
		ServerContext const &operator=(ServerContext const &rhs);

		/* setters */
		void	initLocationBlock(std::string &location);
		void	setDefaultValues(DirectiveVector &vector, std::string key, std::string value);
		void	setValue(DirectiveVector &vector, std::string key, std::string value);
		void	setListenValues(std::string key, std::string line, DirectiveIterator it);
		void	setDefaultValuePort(void);
		void	setPort(void);
		void	setIpAddress(void);
		void	setBlockIndex(size_t index);

		/* getters */
		VectorString const	getValue(std::string location, std::string directive);
		DirectiveVector		&getDirectiveVector(std::string location_name);
		int	const			&getPort(void) const;
		uint32_t const		&getIpAddress(void) const;
		size_t				&getBlockIndex(void);

		/* validity check functions */
		bool	isValidDirective(std::string directive) const;
		bool	isExistingLocat(std::string location) const;

		/* display functions */
		void	printInfoServer(void) const;

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

		std::string	index_location_tmp;

	private:
		int					m_port;
		uint32_t			m_ip_address;
		DirectiveVector		m_serv_directives;
		MapLocation			m_locat_directives;
		size_t				m_block_index;
		
};

#endif // SERVERCONTEXT_HPP