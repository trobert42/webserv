/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerContext.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:00 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 12:54:26 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerContext.hpp"

ServerContext::ServerContext(void) : m_port (0), m_ip_address(0), m_serv_directives(), m_locat_directives(), m_block_index(0)
{
	setDefaultValues(m_serv_directives, "port", "");
	setDefaultValues(m_serv_directives, "ip address", "0.0.0.0");
	setDefaultValues(m_serv_directives, "server_name", "webserv");
	setDefaultValues(m_serv_directives, "root", "html");
	setDefaultValues(m_serv_directives, "index", "");
	setDefaultValues(m_serv_directives, "autoindex", "off");
	setDefaultValues(m_serv_directives, "client_max_body_size", "1");
	setDefaultValues(m_serv_directives, "error_page", "");
	setDefaultValues(m_serv_directives, "allowed_methods", "GET POST DELETE");
	setDefaultValues(m_serv_directives, "rewrite", "");
	setDefaultValues(m_serv_directives, "cgi", "on");
	setDefaultValues(m_serv_directives, "uploaded_files_dir", "/assets/uploaded_files/");
	setIpAddress();
}

ServerContext::~ServerContext(void) {}

ServerContext::ServerContext(ServerContext const &cpy) { *this = cpy; }

ServerContext const &ServerContext::operator=(ServerContext const &rhs)
{
	if (this != &rhs)
	{
		m_locat_directives = rhs.m_locat_directives;
		m_serv_directives = rhs.m_serv_directives;
		index_location_tmp = rhs.index_location_tmp;
		m_port = rhs.m_port;
		m_ip_address = rhs.m_ip_address;
		m_block_index = rhs.m_block_index;
	}
	return *this;
}

void	ServerContext::initLocationBlock(std::string &location)
{
	DirectiveVector new_location_block;

	setDefaultValues(new_location_block, "index", "");
	setDefaultValues(new_location_block, "autoindex", "off");
	setDefaultValues(new_location_block, "root", "");
	setDefaultValues(new_location_block, "client_max_body_size", "");
	setDefaultValues(new_location_block, "error_page", "");
	setDefaultValues(new_location_block, "allowed_methods", "GET POST DELETE");
	setDefaultValues(new_location_block, "uploaded_files_dir", "");
	setDefaultValues(new_location_block, "rewrite", "");
	setDefaultValues(new_location_block, "cgi", "on");
	
	m_locat_directives[location] = new_location_block;
}

////////////////////////// SETTERS //////////////////////////

void	ServerContext::setDefaultValues(DirectiveVector &vector, std::string key, std::string value)
{
	Directive toAdd(key, value);
	vector.push_back(toAdd);
}

void	ServerContext::setDefaultValuePort(void)
{
	setValue(m_serv_directives, "port", "8080");
	setPort();
}

void	ServerContext::setListenValues(std::string key, std::string line, DirectiveIterator it)
{
	removeChar(line, " \t");
	size_t count = std::count(line.begin(), line.end(), ':');
	if (count > 1)
		throw InvalidConfFileException("invalid directive syntax");
	size_t colon_found = line.find(":");
	if (colon_found != std::string::npos)
	{
		std::string ip_value = line.substr(0, colon_found);
		std::string port_value = line.substr(colon_found + 1, line.size());
		if (key == "port" && !getValue("serv", "port").empty() && getValue("serv", "port")[0] == port_value)
			throw InvalidConfFileException("can't use same port value");
		if (key == "port" && !port_value.empty())
		{
			it->setValue(port_value);
			setPort();
		}
		else if (key == "ip address")
		{
			it->setValue(ip_value);
			setIpAddress();
		}
		return ;
	}
	size_t dot_found = line.find(".");
	if (dot_found != std::string::npos || line == "localhost")
	{
		it->setValue(line);
		setIpAddress();
	}
	if (strtof(line.c_str(), NULL) > MIN_PORT_RANGE && strtof(line.c_str(), NULL) < MAX_PORT_RANGE)
	{
		it->setValue(line);
		setPort();
	}
}

void	ServerContext::setPort(void)
{
	VectorString port_vector = getValue("serv", "port");
	for (int i = 0; i < (int) port_vector[0].size(); i++)
	{
		if (!isdigit(port_vector[0][i]))
			throw InvalidConfFileException("wrong port value");
	}
	int port_int = atoi(port_vector[0].c_str());
	m_port = port_int;
}

void	ServerContext::setIpAddress(void)
{
	VectorString ip_vector = getValue("serv", "ip address");
	int	tmp = inet_addr(ip_vector[0].c_str());
	if (tmp == -1)
		throw InvalidConfFileException("wrong ip address");
	uint32_t ip = inet_addr(ip_vector[0].c_str());
	m_ip_address = ip;
}

void	ServerContext::setValue(DirectiveVector &vector, std::string key, std::string value)
{
	removeChar(key, " \t");
	DirectiveIterator it = vector.begin();
	if (key == "listen")
	{
		for (; it != vector.end(); ++it)
		{
			if (it->getName() == "port")
				setListenValues("port", value, it);
			if (it->getName() == "ip address")
				setListenValues("ip address", value, it);
		}
		return ;
	}
	it = vector.begin();
	for (; it != vector.end(); ++it)
	{
		if (it->getName() == key)
		{
			if (key == "rewrite" && value.empty())
				throw InvalidConfFileException("empty value for rewrite");
			return (it->setValue(value));
		}
	}
}

void	ServerContext::setBlockIndex(size_t index) { m_block_index = index; }

////////////////////////// GETTERS //////////////////////////

DirectiveVector	&ServerContext::getDirectiveVector(std::string location)
{
	if (!location.empty() && location == "serv")
		return m_serv_directives;
	MapLocationIterator it = m_locat_directives.begin();
	for (; it != m_locat_directives.end(); ++it)
	{
		if (it->first == location)
			return it->second;
	}
	return (it)->second;
}

VectorString const ServerContext::getValue(std::string location, std::string directive)
{
	VectorString empty;
	empty.push_back("");

	if (!location.empty() && location != "serv")
	{
		DirectiveIterator it = m_locat_directives[location].begin();
		for (; it != m_locat_directives[location].end(); ++it)
		{
			if ((directive == it->getName() && !it->getValue().empty()) || \
				(directive == it->getName() && directive == "index" && it->getValue().empty()))
				return it->getValue();
		}
	}
	DirectiveIterator it = m_serv_directives.begin();
	for (; it != m_serv_directives.end(); ++it)
	{
		if (it->getName() == directive)
			return it->getValue();
	}
	return empty;
}

int	const &ServerContext::getPort(void) const { return m_port; }

uint32_t	const &ServerContext::getIpAddress(void) const { return m_ip_address; }

size_t	&ServerContext::getBlockIndex(void) {return m_block_index; }

////////////////////////// VALIDITY CHECK FUNCITONS //////////////////////////

bool	ServerContext::isValidDirective(std::string directive) const
{
	removeChar(directive, " \t");
	if (directive.empty())
		return false;
	for (int i = 0; i < (int) serv_direc_nbr; i++)
	{
		if (directive == directives_server_context[i])
			return true;
	}
	return false;
}

bool	ServerContext::isExistingLocat(std::string location) const
{
	MapLocationConstIterator it = m_locat_directives.begin();
	for (; it != m_locat_directives.end(); ++it)
	{
		if (it->first == location)
			return true;
	}
	return false;
}

////////////////////////// DISPLAY FUNCTIONS //////////////////////////

void	ServerContext::printInfoServer(void) const
{
	DirectiveConstIterator it_serv_direc = m_serv_directives.begin();
	for (; it_serv_direc != m_serv_directives.end(); ++it_serv_direc)
	{
		std::cout << it_serv_direc->getName() << ": ";
		it_serv_direc->printInfoLocatVector();
		std::cout << std::endl;
	}

	MapLocationConstIterator it_map_locat = m_locat_directives.begin();
	for (; it_map_locat != m_locat_directives.end(); ++it_map_locat)
	{
		std::cout << "location: [" << it_map_locat->first << "]" << std::endl;
		DirectiveConstIterator it_direc_vector = it_map_locat->second.begin();
		for (; it_direc_vector != it_map_locat->second.end(); ++it_direc_vector)
		{
			std::cout << "	" << it_direc_vector->getName() << ": ";
			it_direc_vector->printInfoLocatVector();
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}