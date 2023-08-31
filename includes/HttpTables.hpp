/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpTables.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 15:12:52 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 13:39:39 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPTABLES_HPP
#define HTTPTABLES_HPP

#include <string>
#include <vector>
#include <map>
#include <set>

#include "../includes/httpDatas.hpp"

class HttpTables
{
public:
	~HttpTables() { delete instance; }

	static HttpTables*	getInstance();
	const std::string	getStatusDescription(e_status_code code);
	const std::vector<std::string> getMIMETypeExtensions(std::string MIME_type);
	const std::string getMIMEtype(std::string extension);
	bool  isCGIsupported(std::string extension);

private:
	static HttpTables	*instance;
	std::map<e_status_code, std::string>	status_codes_table;
	std::map<std::string, s_mimetype>	MIME_types_table;
	std::set<std::string>	supported_cgi_table;

	HttpTables();
	HttpTables(const HttpTables &) {}
	HttpTables	&operator=(const HttpTables&) { return *this; }

	void	addMIMEType(std::string MIME_type, std::string extension, std::string path_to_script);
};

#endif // HTTPTABLES_HPP