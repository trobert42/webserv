/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:18:31 by trobert           #+#    #+#             */
/*   Updated: 2023/08/08 13:46:50 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <fcntl.h>
#include <vector>
#include <string>
#include <map>
#include <arpa/inet.h>

#include "HttpWorker.hpp"
#include "Webserv.hpp"
#include "ConfigFile.hpp"
#include "CGIHandler.hpp"
#include "Request.hpp"
#include "Connection.hpp"

#define SOCKCREAT_ERR "function socket() failed"
#define SOCKSETOPT_ERR "function setsockopt() failed"
#define SOCKBIND_ERR "function bind() failed"
#define SOCKLISTEN_ERR "function listen() failed"
#define EPOLLCREA_ERR "function epoll_create() failed"
#define EPOLLWAIT_ERR "function epoll_wait() failed"
#define EPOLLCTL_ERR "function epoll_ctl() failed"
#define SOCKACCEPT_ERR "function accept() failed"
#define FCNTL_ERR "function fcntl() failed"
#define GETSOCKNAME_ERR "function getsockname() failed"

#define MAX_EVENTS 3000
#define TIME_OUT 3000
#define REQUEST_TIMEOUT 10
#define CONNECTION_TIMEOUT 10

class Server 
{
	public:
		~Server(void);
		Server(ConfigFile info);
		Server(Server const &cpy);
		Server const &operator=(Server const &rhs);
	
		void		loopConnection(void);

		/* getters */
		std::string	getPortFromFd(int fd);
		std::string	getIpFromFd(int fd);

		/* display functions */
		void		printSocketInfo(int socket, char* addr, in_port_t port) const;

		void		closeAllFds(void);
		void		exitServer(int sig);


		/* exceptions' classes */
		class FailedServerInitException : public std::exception
		{
			public:
				FailedServerInitException(const std::string &message)
				{
					m_message = "in server connexion: ";
					m_message += message;
				}
				virtual ~FailedServerInitException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};

		class BadConnectionException : public std::exception
		{
			public:
				BadConnectionException(const std::string &message)
				{
					m_message = "in socket connexion: ";
					m_message += message;
				}
				virtual ~BadConnectionException() throw() {}
				virtual const char* what() const throw() { return (m_message.c_str()); }

			private:
				std::string m_message;
		};

	private:
		ConfigFile					m_info;
		std::vector<int>			m_serv_fds;
		int				 			m_ep_fd;
		struct sockaddr_in		  	m_serv_addr;
		size_t			  			m_addrlen;
		struct epoll_event			m_events[MAX_EVENTS];
		HttpWorker					http_worker;
		std::map<int, Connection>	connections;

		Server(void);

		/* server connection */
		int			createServerSocket(void);
		void		setSocketOpt(int fd) const;
		void		createSocketStruct(int serv_fd, int serv_port, uint32_t serv_ip);
		void		bindSocket(int serv_fd, int serv_port) const;
		void		putListenSocket(int serv_fd) const;
		void		initEpollHandler(void);
		void		addServersSocketInEpoll(int serv_fd);
		void		closeConnection(int fd);

		/* loop events */
		void		addNewSocket(int new_client, struct sockaddr_in client_addr);
		bool		isSocketServerFd(int serv_fd) const;
		void		handleNewConnection(int serv_fd);
		std::string getFileContents(const std::string &path);
		void		setupConnectionForRead(int event_index);
		void		setupConnectionForWrite(int event_index);
		void		checkConnectionAvailability();
		void		handleReadWriteOperations();
};

#endif // SERVER_HPP