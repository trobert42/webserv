/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/25 20:10:11 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 13:41:02 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/time.h>

#include "../includes/Request.hpp"

double	Request::getRequestAge()
{
	struct timeval end;
	gettimeofday(&end, NULL);
	return (end.tv_sec - created_time.tv_sec) + (end.tv_usec - created_time.tv_usec) / 1000000.0;
}