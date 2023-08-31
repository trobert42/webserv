/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:17:34 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 12:56:19 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ConfigFile.hpp"

ConfigFile::ConfigFile(void) {}

ConfigFile::~ConfigFile(void) {	m_conf_stream.close(); }

ConfigFile::ConfigFile(std::string &conf_name) : m_conf_name(conf_name)
{
	m_conf_stream.open(m_conf_name.c_str(), std::ios_base::in);
}

ConfigFile::ConfigFile(ConfigFile const &cpy) { *this = cpy; }

ConfigFile &ConfigFile::operator=(ConfigFile const &rhs)
{
	if (this != &rhs)
	{
		m_conf_name = rhs.m_conf_name;
		m_nbr_serv_block = rhs.m_nbr_serv_block;
		m_servers = rhs.m_servers;
	}
	return *this;
}

void	ConfigFile::parseInfo(void)
{
	while (m_conf_stream.eof() == false)
	{
		std::string line;
		std::getline(m_conf_stream, line);
		if (isEmpty(line))
			continue ;
		if (isBeginServContext(line))
			parseServContext();
		else
			throw InvalidConfFileException(INVALID_LINE_ERR);
	}
	m_conf_stream.close();
	if (!checkPortServers())
		throw InvalidConfFileException(NO_SERV_ERR);
}

////////////////////////// SERVER CONTEXT //////////////////////////

bool	ConfigFile::isBeginServContext(std::string line) const
{
	removeChar(line, " \t");
	if (line == "server{") 
		return true;
	if (line.find("server", 0) != std::string::npos)
		throw InvalidConfFileException(INVALID_SERV_SYNTAX_ERR);
	return false;
}

bool	ConfigFile::isEndServContext(std::string line) const
{
	removeChar(line, " \t");
	if (line == "}")
		return true;
	if (line.find("}", 0) != std::string::npos)
		throw InvalidConfFileException(INVALID_SERV_SYNTAX_ERR);
	return false;
}

void	ConfigFile::parseServContext(void)
{
	ServerContext	serv_block;
	bool			end_of_block = false;

	while (m_conf_stream.eof() == false || end_of_block == true)
	{
		std::string line;
		std::getline(m_conf_stream, line);
		if (isEmpty(line))
			continue ;
		if (isEndServContext(line))
		{
			end_of_block = true;
			break ;
		}
		if (isBeginLocatContext(serv_block, line))
		{
			parseLocatContext(serv_block);
			continue ;
		}
		addInfoServContext(serv_block, line);
	}
	if (end_of_block == false)
		throw InvalidConfFileException(INVALID_SERV_SYNTAX_ERR);
	if (serv_block.getPort() == 0)
		serv_block.setDefaultValuePort();
	m_servers.push_back(serv_block);
}

void   ConfigFile::addInfoServContext(ServerContext &serv_block, std::string line)
{
	size_t ret = line.find(':', 0);
	if (ret == std::string::npos)
		throw InvalidConfFileException(MISSING_COLON_ERR);
	std::string key = line.substr(0, ret);
	std::string values = line.substr(ret + 1, line.size());
	if (!serv_block.isValidDirective(key))
		throw InvalidConfFileException(UNKNOWN_DIRECTIVES_ERR);
	serv_block.setValue(serv_block.getDirectiveVector("serv"), key, values);
	m_nbr_serv_block++;
}

////////////////////////// LOCATION CONTEXT //////////////////////////

bool	ConfigFile::isBeginLocatContext(ServerContext &serv_block, std::string line)
{
	if (line.find("location") == std::string::npos)
		return false;

	std::vector<std::string> splited = splitVector(line, ' ');
	if (splited.size() != 3)
		throw InvalidConfFileException(INVALID_LOCAT_SYNTAX_ERR);
	if (splited[0] != "location" || splited[1].size() <= 0 || splited[2] != "{")
		throw InvalidConfFileException(INVALID_LOCAT_SYNTAX_ERR);
	serv_block.initLocationBlock(splited[1]);
	serv_block.index_location_tmp = splited[1];
	return true;
}

bool	ConfigFile::isEndLocatContext(std::string line) const
{
	removeChar(line, " \t");
	if (line == "}")
		return true;
	if (line.find("}", 0) != std::string::npos)
		throw InvalidConfFileException(INVALID_LOCAT_SYNTAX_ERR);
	return false;
}

void	ConfigFile::parseLocatContext(ServerContext &serv_block)
{
	while (m_conf_stream.eof() == false)
	{
		std::string line;
		std::getline(m_conf_stream, line);
		if (isEmpty(line))
			continue ;
		if (isEndLocatContext(line))
			break ;
		removeChar(line, "\t");
		size_t ret = line.find(':', 0);
		if (ret == std::string::npos)
			throw InvalidConfFileException(MISSING_COLON_ERR);
		addInfoLocatVector(serv_block, line);
	}
}

void	ConfigFile::addInfoLocatVector(ServerContext &serv_block, std::string line)
{
	size_t		ret = line.find(':', 0);
	if (ret == std::string::npos)
		throw InvalidConfFileException(MISSING_COLON_ERR);
	std::string	key = line.substr(0, ret);
	std::string	value = line.substr(ret + 1, line.size());
	if (!serv_block.isValidDirective(key))
		throw InvalidConfFileException(UNKNOWN_DIRECTIVES_ERR);
	else if (key == "listen" || key == "server_name")
		throw InvalidConfFileException(BAD_DIRECTIVE_ERR);
	serv_block.setValue(serv_block.getDirectiveVector(serv_block.index_location_tmp), key, value);
}

////////////////////////// GETTERS //////////////////////////

VectorServers	ConfigFile::getServerContexts(std::string port)
{
	VectorServers	vector;
	VectorServersIterator it = m_servers.begin();
	for (; it != m_servers.end(); ++it)
	{
		if (it->getValue("serv", "port")[0] == port)
			vector.push_back(*it);
	}
	return vector;
}

VectorServers	&ConfigFile::getServersVector(void)
{
	return m_servers;
}


////////////////////////// DISPLAY //////////////////////////

void	ConfigFile::printInfoServers(void)
{
	VectorServersConstIterator it = m_servers.begin();
	size_t  index_server_block = 0;
	for (; it != m_servers.end(); ++it)
	{
		std::cout << "---- Server block n" << index_server_block + 1 << " ----" << std::endl;
		m_servers[index_server_block].printInfoServer();
		m_servers[index_server_block].setBlockIndex(index_server_block + 1);
		index_server_block++;
		std::cout << std::endl << std::endl;
	}
}

bool ConfigFile::checkPortServers(void)
{
	std::vector<int>	port_server;
	VectorServersConstIterator it = m_servers.begin();
	for (; it != m_servers.end(); ++it)
		port_server.push_back(it->getPort());
	m_nbr_serv_block = port_server.size();
	if (m_nbr_serv_block < 1)
		return false;
	return true;
}