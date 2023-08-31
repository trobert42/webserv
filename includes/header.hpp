/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/22 18:27:58 by agras             #+#    #+#             */
/*   Updated: 2023/08/07 11:04:02 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <string>
#include <stdint.h>
#include <cmath>
#include <sstream>
#include <algorithm>

#include "../includes/utils.hpp"

class HeaderField
{
public:
	HeaderField(std::string name) : name(tolowerString(name)) {}
	HeaderField(const HeaderField& other) : name(other.name) {}
	virtual	~HeaderField() {}

	virtual std::string		getName() const { return name; };
	virtual std::string		getValue() const = 0;
	virtual HeaderField*	clone() const = 0;

protected:
	std::string	name;
};


#endif // HEADER_HPP