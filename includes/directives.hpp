/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   directives.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:24 by trobert           #+#    #+#             */
/*   Updated: 2023/08/03 00:18:20 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVES_HPP
# define DIRECTIVES_HPP

#include <string>

const size_t serv_direc_nbr = 11;
const std::string directives_server_context[] = {
	"listen",
	"server_name",
	"root",
	"index",
	"autoindex",
	"client_max_body_size",
	"allowed_methods",
	"error_page",
	"rewrite",
	"cgi",
	"uploaded_files_dir",
};

const size_t locat_direc_nbr = 9;
const std::string directives_location_context[] = {
	"index",
	"root",
	"autoindex",
	"client_max_body_size",
	"allowed_methods",
	"error_page",
	"rewrite",
	"uploaded_files_dir",
	"cgi",
};

#endif // DIRECTIVES_HPP