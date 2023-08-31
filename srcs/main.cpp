/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:17:49 by trobert           #+#    #+#             */
/*   Updated: 2023/08/02 19:28:48 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserv.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv [config_file]" << std::endl;
		return 1;
	}
	else
	{
		try
		{
			Webserv webserv(av[1]);
			webserv.run();
		}
		catch(const std::exception &e)
		{
			std::cerr << RED << "Error: " << e.what() << WHITE << std::endl;
		}
	}
	return 0;
}