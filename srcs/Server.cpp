/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/06 15:17:56 by trobert           #+#    #+#             */
/*   Updated: 2023/08/08 17:36:54 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>

#include "../includes/Server.hpp"

Server::Server(void) {}

Server::~Server(void)
{
	closeAllFds();
}

Server::Server(ConfigFile info) : m_info(info),  m_ep_fd(-1)
{
	try
	{
		VectorServers servers = m_info.getServersVector();
		VectorServersIterator it = servers.begin();
		for (; it != servers.end(); ++it)
		{
			int serv_port = it->getPort();
			uint32_t serv_ip = it->getIpAddress();

			int serv_fd = createServerSocket();
			setSocketOpt(serv_fd);
			createSocketStruct(serv_fd, serv_port, serv_ip);
			bindSocket(serv_fd, serv_port);
			putListenSocket(serv_fd);
		}
		initEpollHandler();
	}
	catch(const std::exception &e)
	{
		closeAllFds();
		throw Webserv::FailedConnectionServerException(e.what());
	}
}

Server::Server(Server const &cpy) { *this = cpy; }

Server const &Server::operator=(Server const &rhs)
{
	if (this != &rhs)
	{
		m_info = rhs.m_info;
		m_serv_fds = rhs.m_serv_fds;
		m_ep_fd = rhs.m_ep_fd;
		m_serv_addr = rhs.m_serv_addr;
		m_addrlen = rhs.m_addrlen;
		m_events[MAX_EVENTS] = rhs.m_events[MAX_EVENTS];
	}
	return *this;
}

int	Server::createServerSocket(void)
{
	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	if (serverfd < 0)
		throw FailedServerInitException(SOCKCREAT_ERR);
	m_serv_fds.push_back(serverfd);
	return serverfd;
}

void	Server::setSocketOpt(int fd) const
{
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
		throw FailedServerInitException(SOCKSETOPT_ERR);
}

void	Server::createSocketStruct(int serv_fd, int serv_port, uint32_t serv_ip)
{
	m_addrlen = sizeof(m_serv_addr);
	memset((char *) &m_serv_addr, 0, m_addrlen);
	m_serv_addr.sin_family = AF_INET;
	m_serv_addr.sin_addr.s_addr = serv_ip;
	m_serv_addr.sin_port = htons(serv_port);
	std::cout << "Server socket information :" << std::endl;
	printSocketInfo(serv_fd, inet_ntoa(m_serv_addr.sin_addr), m_serv_addr.sin_port);
}

void	Server::bindSocket(int serv_fd, int serv_port) const
{
	if (bind(serv_fd, (struct sockaddr *) &m_serv_addr, m_addrlen) < 0)
	{
		std::stringstream ss;
		ss << "function bind() failed for port : ";
		ss << serv_port;
		std::string msg = ss.str();
		throw FailedServerInitException(msg);
	}
}

void	Server::putListenSocket(int serv_fd) const
{
	if (listen(serv_fd, SOMAXCONN) < 0)
		throw FailedServerInitException(SOCKLISTEN_ERR);
}

void	Server::addServersSocketInEpoll(int serv_fd)
{
	struct epoll_event	ev;
	memset((char *)&ev, 0, sizeof(ev)); 
	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = serv_fd;
	if (epoll_ctl(m_ep_fd, EPOLL_CTL_ADD, serv_fd, &ev) < 0)
		throw FailedServerInitException(EPOLLCTL_ERR);
}

void	Server::initEpollHandler(void)
{
	m_ep_fd = epoll_create(1);
	if (m_ep_fd < 0)
		throw FailedServerInitException(EPOLLCREA_ERR);
	std::vector<int>::iterator it = m_serv_fds.begin();
	for (; it != m_serv_fds.end(); ++it)
		addServersSocketInEpoll(*it);
}

void	Server::exitServer(int)
{
	closeAllFds();
	exit(0);
}

////////////////////////// LOOP EVENTS //////////////////////////

void	Server::loopConnection(void)
{
	while (true)
	{
		handleReadWriteOperations();
		checkConnectionAvailability();
	}
}

void	Server::handleReadWriteOperations()
{
	int event_count = epoll_wait(m_ep_fd, m_events, MAX_EVENTS, TIME_OUT);
	if (event_count == -1)
		throw FailedServerInitException(EPOLLWAIT_ERR);
	for (int i = 0; i < event_count;)
	{
		try
		{
			if (m_events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				closeConnection(m_events[i].data.fd);
			else if (isSocketServerFd(m_events[i].data.fd))
				handleNewConnection(m_events[i].data.fd);
			else
			{
				if (m_events[i].events & EPOLLIN)
				{
					connections[m_events[i].data.fd].readFromSocket();
					http_worker.handleRequest(connections[m_events[i].data.fd], getIpFromFd(m_events[i].data.fd));
					if (connections[m_events[i].data.fd].isBufferReadyToWrite())
						setupConnectionForWrite(i);
				}
				else if (m_events[i].events & EPOLLOUT)
				{
					connections[m_events[i].data.fd].writeToSocket();
					if (!connections[m_events[i].data.fd].isBufferReadyToWrite())
						setupConnectionForRead(i);
				}
			}
			i++;
		}
		catch(const std::exception &e)
		{
			std::cerr << RED << e.what() << WHITE << std::endl;
			closeConnection(m_events[i].data.fd);
			i++;
		}
	}
}

void	Server::checkConnectionAvailability()
{
	for (std::map<int, Connection>::iterator it = connections.begin(); it != connections.end();)
	{
		try
		{
			if (it->second.getRequestCount() == 0)
			{
				if (it->second.getLastTimeSinceRequest() > CONNECTION_TIMEOUT)
				{
					std::map<int, Connection>::iterator tmp = it;
					it++;
					closeConnection(tmp->second.getFd());
				}
				else
					it++;
			}
			else if (it->second.getNextRequest().getRequestAge() > REQUEST_TIMEOUT)
			{
				http_worker.respondError(it->second, it->second.getNextRequest(), Request::InvalidHttpRequest("request timeout, REQUEST TIMEOUT 408 ERROR", RequestTimeout));
				it->second.writeToSocket();
				std::map<int, Connection>::iterator tmp = it;
				it++;
				closeConnection(tmp->second.getFd());
			}
			else
				it++;
		}
		catch(const std::exception& e)
		{
			std::map<int, Connection>::iterator tmp = it;
			it++;
			closeConnection(tmp->second.getFd());
		}
	}
}

void	Server::setupConnectionForRead(int event_index)
{
	m_events[event_index].events = EPOLLIN | EPOLLRDHUP;
	if (epoll_ctl(m_ep_fd, EPOLL_CTL_MOD, m_events[event_index].data.fd, &m_events[event_index]) < 0)
		throw BadConnectionException(EPOLLCTL_ERR);
}

void	Server::setupConnectionForWrite(int event_index)
{
	m_events[event_index].events = EPOLLOUT | EPOLLRDHUP;
	if (epoll_ctl(m_ep_fd, EPOLL_CTL_MOD, m_events[event_index].data.fd, &m_events[event_index]) < 0)
		throw BadConnectionException(EPOLLCTL_ERR);
}

bool	Server::isSocketServerFd(int serv_fd) const
{
	std::vector<int>::const_iterator it = m_serv_fds.begin();
	for (; it != m_serv_fds.end(); ++it)
	{
		if (*it == serv_fd)
			return true;
	}
	return false;
}

void	Server::addNewSocket(int new_client, struct sockaddr_in client_addr)
{
	setSocketOpt(new_client);
	struct epoll_event	ev;

	memset((char *)&ev, 0, sizeof(ev)); 
	ev.events = EPOLLIN | EPOLLRDHUP;
	ev.data.fd = new_client;
	if (epoll_ctl(m_ep_fd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0)
		throw BadConnectionException(EPOLLCTL_ERR);
	else
	{
		std::cout << YELLOW << "New connection established" << WHITE << std::endl;
		printSocketInfo(new_client, inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
		std::cout << std::endl;
	}
}

void	Server::handleNewConnection(int serv_fd)
{
	struct sockaddr_in	client_addr;
	socklen_t addr_len = sizeof(client_addr);

	memset((char *)&client_addr, 0, sizeof(client_addr));
	int new_socket_fd = accept(serv_fd, (struct sockaddr*) &client_addr, &addr_len);
	if (new_socket_fd < 0)
		throw BadConnectionException(SOCKACCEPT_ERR);
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		throw BadConnectionException(SOCKACCEPT_ERR);
	connections[new_socket_fd] = Connection(new_socket_fd, m_info.getServerContexts(getPortFromFd(new_socket_fd)));
	if (fcntl(new_socket_fd, F_SETFL, O_NONBLOCK) < 0)
		throw BadConnectionException(FCNTL_ERR);
	addNewSocket(new_socket_fd, client_addr);
}

std::string Server::getFileContents(const std::string &path)
{
	std::ifstream file(path.c_str());
	if (!file)
		return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

std::string	Server::getPortFromFd(int socket)
{
	struct sockaddr_in	client;
	std::ostringstream	oss;
	socklen_t			addr_len;
	uint16_t			port;

	memset(&client, 0, sizeof(client));
	addr_len = sizeof(client);
	if (getsockname(socket, (struct sockaddr *) &client, &addr_len) < 0)
		throw BadConnectionException(GETSOCKNAME_ERR);
	port = htons(client.sin_port);
	oss << port;
	return oss.str();
}

std::string	Server::getIpFromFd(int socket)
{
	struct sockaddr_in client;
	socklen_t addr_len = sizeof(client);

	if (getsockname(socket, (struct sockaddr *) &client, &addr_len) < 0)
		throw BadConnectionException(GETSOCKNAME_ERR);

	std::string ip = inet_ntoa(client.sin_addr);
	return ip;
}

void	Server::closeConnection(int fd)
{
	if (connections.count(fd) == 0)
	{
		std::cerr << "Could not close connection. Already closed." << std::endl;
		return;
	}
	epoll_ctl(m_ep_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	std::cout << RED << "Connection closed for client [" << fd << "]" << WHITE << std::endl;
	connections.erase(fd);
}

void	Server::closeAllFds(void)
{
	std::vector<int>::const_iterator it = m_serv_fds.begin();
	for (; it != m_serv_fds.end(); ++it)
	{
		std::cout << RED << "Connection closed for server fd [" << *it << "]" << WHITE << std::endl;
		close(*it);
	}
	if (m_ep_fd != -1)
	{
		std::cout << RED << "Connection closed for m_ep_fd [" << m_ep_fd << "]" << WHITE << std::endl;
		close(m_ep_fd);
	}
	for (std::map<int, Connection>::iterator ito = connections.begin(); ito != connections.end();)
	{
		std::map<int, Connection>::iterator tmp = ito;
		ito++;
		closeConnection(tmp->first);
	}
}

void	Server::printSocketInfo(int socket, char* addr, in_port_t port) const
{
	std::cout << GREY << "Socket fd : [" << socket << "]" << std::endl
		<< "Address IP : [" << addr << "]" << std::endl
		<< "Port : [" << ntohs(port) << "]" << std::endl << WHITE << std::endl;
}