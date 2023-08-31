/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 20:33:01 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 18:29:02 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>
#include <fcntl.h>

#include "../includes/Connection.hpp"

Connection::Connection() : fd(0), received_new_bytes(false), contexts()
{
	read_buffer.head_pos = 0;
	write_buffer.head_pos = 0;
	gettimeofday(&last_time_since_request, NULL);
}

Connection::Connection(int fd, VectorServers context) : fd(fd), received_new_bytes(false), contexts(context)
{
	read_buffer.head_pos = 0;
	write_buffer.head_pos = 0;
	gettimeofday(&last_time_since_request, NULL);
	requests.push(Request());
}

Connection&	Connection::operator=(const Connection& rhs)
{
	if (this == &rhs)
		return *this;
	this->fd = rhs.fd;
	this->contexts = rhs.contexts;
	this->read_buffer = rhs.read_buffer;
	this->write_buffer = rhs.write_buffer;
	this->requests = rhs.requests;
	this->last_time_since_request = rhs.last_time_since_request;
	this->received_new_bytes = rhs.received_new_bytes;
	return *this;
}

Connection::Connection(const Connection& other)
{
	*this = other;
}

Connection::~Connection()
{
}

void	Connection::createNewRequest()
{
	requests.push(Request());
	gettimeofday(&last_time_since_request, NULL);
}

void	Connection::removeRequest()
{
	requests.pop();
}

void	Connection::reset_read_buffer()
{
	read_buffer.data.clear();
	read_buffer.head_pos = 0;
}

void	Connection::reset_write_buffer()
{
	write_buffer.data.clear();
	write_buffer.head_pos = 0;
}

bool	Connection::isBufferReadyToWrite()
{
	if (write_buffer.data.size() > write_buffer.head_pos)
		return true;
	return false;
}

void	Connection::readFromSocket()
{
	char buffer[SOCKET_READ_BUFF_SIZE];
	int  bytes_received = SOCKET_READ_BUFF_SIZE;
	for (int i = 0; bytes_received == SOCKET_READ_BUFF_SIZE; i++)
	{
		bytes_received = recv(fd, &buffer, SOCKET_READ_BUFF_SIZE, 0);
		if (bytes_received < 0)
			throw SocketConnexionException(RECV_ERR);
		if (bytes_received == 0)
			return ;
		std::cout << BLUE << "Received " << bytes_received << " bytes from client " << fd << WHITE <<std::endl;
		for (int i = 0; i < bytes_received; i++)
			read_buffer.data.push_back(buffer[i]);
		received_new_bytes = true;
	}
}

void	Connection::writeToSocket()
{
	int bytes_sent = send(fd, write_buffer.data.data() + write_buffer.head_pos, write_buffer.data.size() - write_buffer.head_pos, 0);
	std::cout << MAGENTA << "Sent " << bytes_sent << " bytes to client " << fd << WHITE <<std::endl;
	if (bytes_sent < 0)
		throw SocketConnexionException(SEND_ERR);
	write_buffer.head_pos += bytes_sent;
	if (write_buffer.head_pos >= WRITE_BUFF_FREE_THRESHOLD)
	{
		write_buffer.data.erase(write_buffer.data.begin(), write_buffer.data.begin() + write_buffer.head_pos);
		write_buffer.head_pos = 0;
	}
	if (bytes_sent == 0)
		throw SocketConnexionException(SEND_ERR);
}

ServerContext	&Connection::getSpecificContext(std::string ip)
{
	if (getNextRequest().header_fields.count("host") > 0)
	{
		for (VectorServersIterator it = contexts.begin(); it != contexts.end(); ++it)
		{
			if (it->getValue("serv", "server_name")[0] == getNextRequest().header_fields["host"]->getValue() && it->getValue("serv", "ip address")[0] == ip)
				return *it;
		}
		for (VectorServersIterator it = contexts.begin(); it != contexts.end(); ++it)
		{
			if (it->getValue("serv", "server_name")[0] == getNextRequest().header_fields["host"]->getValue() && it->getValue("serv", "ip address")[0] == "0.0.0.0")
				return *it;
		}
	}
	for (VectorServersIterator it = contexts.begin(); it != contexts.end(); ++it)
	{
		if (it->getValue("serv", "ip address")[0] == ip)
			return *it;
	}
	for (VectorServersIterator it = contexts.begin(); it != contexts.end(); ++it)
	{
		if (it->getValue("serv", "ip address")[0] == "0.0.0.0")
			return *it;
	}
	return *contexts.begin();
}

double		Connection::getLastTimeSinceRequest()
{
	struct timeval end;
	gettimeofday(&end, NULL);
	return (end.tv_sec - last_time_since_request.tv_sec) + (end.tv_usec - last_time_since_request.tv_usec) / 1000000.0;
}
