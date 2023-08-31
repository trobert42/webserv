/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 20:17:23 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 13:45:35 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <vector>
#include <queue>
#include <sys/time.h> 

#include "../includes/httpDatas.hpp"
#include "../includes/Request.hpp"
#include "../includes/ConfigFile.hpp"

#define RECV_ERR "function recv() failed"
#define SEND_ERR "function send() failed"
#define SOCKET_READ_BUFF_SIZE 8192
#define WRITE_BUFF_FREE_THRESHOLD 8192


class Connection
{
public:
	Connection();
	Connection(int fd, VectorServers context);
	Connection(const Connection& other);
	~Connection();
	
	Connection&	operator=(const Connection& rhs);

	void			reset_write_buffer();
	void			reset_read_buffer();
	void			readFromSocket();
	void			writeToSocket();
	bool			isBufferReadyToWrite();
	void			createNewRequest();
	void			removeRequest();
	int				getFd() { return fd; }
	Request&		getNextRequest() { if (requests.empty()) throw std::runtime_error("no request") ; return requests.front(); }
	bool			hasNewBytes() { return received_new_bytes; }
	size_t			getRequestCount() { return requests.size(); }
	ServerContext	&getSpecificContext(std::string ip);
	double			getLastTimeSinceRequest();

	class SocketConnexionException : public std::exception
	{
		public:
			SocketConnexionException(const std::string &message)
			{
				m_message = "in socket connexion: ";
				m_message += message;
			}
			virtual ~SocketConnexionException() throw() {}
			virtual const char* what() const throw() { return (m_message.c_str()); }

		private:
			std::string m_message;
	};

	friend class HttpWorker;

private:
	int					fd;
	bool				received_new_bytes;
	s_ReqBuffer			read_buffer;
	s_ReqBuffer			write_buffer;
	VectorServers		contexts;
	std::queue<Request>	requests;
	struct timeval		last_time_since_request;
};

#endif // CONNECTION_HPP