/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:19 by trobert           #+#    #+#             */
/*   Updated: 2023/08/08 13:49:50 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

#include "utils.hpp"
#include "directives.hpp"
#include "httpDatas.hpp"

#define BAD_DIR_VALUE_ERR "wrong value for directive"
#define MIN_PORT_RANGE 1024
#define MAX_PORT_RANGE 49151
#define MIN_CLIENT_BODY_SIZE 0
#define MAX_CLIENT_BODY_SIZE 100000000

class Directive
{
	public:
		Directive(void);
		~Directive(void);
		Directive(std::string &key, std::string &value);
		Directive(Directive const &cpy);
		Directive const &operator=(Directive const &rhs);

		/* setters */
		void	setValue(std::string &value);

		/* getters */
		std::string const 	&getName(void) const;
		VectorString 	 	&getValue(void);

		/* checks validity */
		bool	isValidValue(std::string &key, VectorString &vector_value);
		void	removeDuplicateVector(VectorString &vector_value);
		/* display */
		void	printInfoLocatVector(void) const;

		/* exceptions' classes */
		class InvalidDirectiveException : public std::exception
		{
			public:
				InvalidDirectiveException(const std::string &message, const std::string &directive)
				{
					m_message = "invalid directive \"" + directive + "\" in config file: ";
					m_message += message;
				}
				virtual ~InvalidDirectiveException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};


	private:
		std::string		m_name;
		VectorString	m_value;
};

#endif // DIRECTIVE_HPP