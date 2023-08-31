/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:07 by trobert           #+#    #+#             */
/*   Updated: 2023/08/02 19:42:36 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserv.hpp"

Webserv::Webserv(void) {}

Webserv::~Webserv(void) {}

Webserv::Webserv(char *av) : m_conf_name(av)
{
	if (m_conf_name == "config_files/default.conf" || !isValidConfFile())
		getDefaultConfFile();
	ConfigFile conf(m_conf_name);
	conf.parseInfo();
	conf.printInfoServers();
	m_conf_file = conf;
	std::cout << "Constructor called with [" << m_conf_name << "]" << std::endl;
}

Webserv::Webserv(Webserv const &cpy) {	*this = cpy; }

Webserv const &Webserv::operator=(Webserv const &rhs)
{
	if (this != &rhs)
	{
		m_conf_name = rhs.m_conf_name;
		m_conf_file = rhs.m_conf_file;
	}
	return *this;
}

bool Webserv::isValidConfFile(void)
{
	if (m_conf_name.empty())
	{
		std::cerr << "nb: arg's empty" << std::endl;
		return false;
	}
	std::string extension = ".conf";
	if (m_conf_name.size() <= extension.size() || \
		m_conf_name.compare(m_conf_name.size() - extension.size(), extension.size(), extension) != 0)
	{
		std::cerr << "nb: .conf not found or invalid" << std::endl;
		return false;
	}
	std::ifstream file;
	file.open(m_conf_name.c_str(), std::ios_base::in);
	if (!file.is_open())
	{
		std::cerr << "nb: can't open or didn't find the file [" << m_conf_name << "]" << std::endl;
		return false;
	}
	file.close();
	return true;
}

void	Webserv::getDefaultConfFile(void)
{
	std::cerr << "Trying default file configuration..." << std::endl;
	std::ifstream defaultFile;
	defaultFile.open("config_files/default.conf", std::ios_base::in);
	if (!defaultFile.is_open() || opendir("config_files/default.conf") != NULL)
		throw InvalidDefaultConfFileException();
	m_conf_name = "config_files/default.conf";
}

void	Webserv::run(void)
{
	Server serverHTTP(m_conf_file);
	serverHTTP.loopConnection();
}