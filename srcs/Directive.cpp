/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/21 16:15:54 by trobert           #+#    #+#             */
/*   Updated: 2023/08/04 14:03:30 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Directive.hpp"

Directive::Directive(void) {}

Directive::~Directive(void) {}

Directive::Directive(std::string &key, std::string &value)
{
	m_name = key;
	m_value = splitVector(value, ' ');
}

Directive::Directive(Directive const &cpy) { *this = cpy; }

Directive const &Directive::operator=(Directive const &rhs)
{
	if (this != &rhs)
	{
		m_name = rhs.m_name;
		m_value = rhs.m_value;
	}
	return *this;
}

////////////////////////// SETTERS //////////////////////////

void	Directive::setValue(std::string &value)
{
	if (isEmpty(value))
		return ;
	VectorString toAdd = splitVector(value, ' ');

	if (isValidValue(m_name, toAdd))
	{
		if (m_name == "allowed_methods")
		{
			std::string methods[] = {"GET", "POST", "DELETE"};
			VectorString methods_list(methods, methods + sizeof(methods) / sizeof(std::string));
			for (VectorIterator it = toAdd.begin(); it != toAdd.end(); ++it)
			{
				if (std::find(methods_list.begin(), methods_list.end(), *it) == methods_list.end())
					throw InvalidDirectiveException(BAD_DIR_VALUE_ERR, m_name);
			}
		}
		if (m_name == "error_page" && !getValue().empty())
		{
			VectorString existing = getValue();
			for (VectorIterator it = existing.begin(); it != existing.end(); ++it)
			{
				if (std::find(toAdd.begin(), toAdd.end(), *it) != toAdd.end())
				{
					existing.erase(it);
					existing.erase(it++);
				}
				else
				{
					VectorString newVector = existing;
					newVector.push_back(toAdd[0]);
					newVector.push_back(toAdd[1]);
					m_value = newVector;
				}
				return;
			}
		}
		removeDuplicateVector(toAdd);
		m_value = toAdd;
	}
	else
		throw InvalidDirectiveException(BAD_DIR_VALUE_ERR, m_name);
}

////////////////////////// GETTERS //////////////////////////

std::string const	&Directive::getName(void) const { return this->m_name; }

VectorString		&Directive::getValue(void) { return this->m_value; }

////////////////////////// VALIDITY CHECK FUNCTIONS //////////////////////////

void	Directive::removeDuplicateVector(VectorString &vector_value)
{
	VectorIterator it = vector_value.begin();
	while (it != vector_value.end())
	{
		VectorIterator found_it = std::find(vector_value.begin(), it, *it);
		if (found_it == it)
			++it;
		else
			it = vector_value.erase(it);
	}
}

bool	Directive::isValidValue(std::string &key, VectorString &vector_value)
{
	std::string value = vector_value[0];
	if (key == "cgi" || key == "autoindex")
	{
		if (vector_value.size() > 1)
			return false;
		if (value != "on" && value != "off")
			return false;
	}
	if (key == "error_page")
	{
		if (vector_value.size() != 2)
			return false;
		int value_int = atoi(value.c_str());
		for (int i = Continue; i <= NetworkAuthenticationRequired; i++)
		{
			if (value_int == i)
				return true;
		}
		return false;
	}
	if (key == "rewrite")
	{
		if (vector_value.size() != 2)
			return false;
		int value_int = atoi(value.c_str());
		for (int i = Continue; i <= NetworkAuthenticationRequired; i++)
		{
			if (value_int == i)
				return true;
		}
		return false;
	}
	if (key == "client_max_body_size")
	{
		if (vector_value.size() > 1)
			return false;
		if (strtod(value.c_str(), NULL) <= MIN_CLIENT_BODY_SIZE || strtod(value.c_str(), NULL) > MAX_CLIENT_BODY_SIZE)
			return false;
		for (int i = 0; i < (int) value.size(); i++)
		{
			if (!isdigit(value[i]))
				return false;
		}
	}
	if (key == "port")
	{
		if (strtod(value.c_str(), NULL) < MIN_PORT_RANGE || strtod(value.c_str(), NULL) > MAX_PORT_RANGE)
			return false;
	}
	if (key == "ip address")
	{
		if (value == "localhost")
			vector_value[0] = "127.0.0.1";
	}
	if ((key == "root" || key == "uploaded_files_dir" || key == "index" || key == "server_name") && vector_value.size() > 1)
		return false;
	return true;
}

////////////////////////// DISPLAY FUNCTIONS //////////////////////////

void	Directive::printInfoLocatVector(void) const
{
	VectorString::const_iterator it = m_value.begin();
	for (; it != m_value.end(); ++it)
	{
		std::cout << "[" << *it << "] ";
	}
}