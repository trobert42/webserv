/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/17 17:46:25 by trobert           #+#    #+#             */
/*   Updated: 2023/08/08 20:03:24 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGIHandler.hpp"

CGIHandler::CGIHandler(void) {}

CGIHandler::CGIHandler(Request& request, std::string path, std::string location) :
	m_request(request),
	m_path(path),
	m_env(NULL),
	m_body_request(request.body),
	m_body_size(request.body.size()),
	m_location(location)
{
	memset(m_args, 0, 3 * sizeof(char *));
	m_body_response.clear();
	initEnv();
	executeCGI();
}

CGIHandler::~CGIHandler(void)
{
	if (m_env)
	{
		for(int i = 0; i < (int) m_map_env.size(); i++)
			free(m_env[i]);
		free(m_env);
	}
}

CGIHandler::CGIHandler(CGIHandler const & cpy)
{
	*this = cpy;
}

CGIHandler const &CGIHandler::operator=(CGIHandler const & rhs)
{
	if (this != &rhs)
	{
		m_request = rhs.m_request;
		m_path = rhs.m_path;
		m_map_env = rhs.m_map_env;
		m_env = rhs.m_env;
		m_body_request = rhs.m_body_request;
		m_body_response = rhs.m_body_response;
		m_body_size = rhs.m_body_size;
		m_location = rhs.m_location;
	}
	return *this;
}


void	CGIHandler::initEnv(void)
{
	if (m_request.header_fields.count("host") > 0 && !m_request.header_fields["host"]->getValue().empty())
		VectorString host = splitVector2(m_request.header_fields["host"]->getValue(), " :\r");
	else
		m_map_env["SERVER_NAME"] = m_request.context.getValue("serv", "server_name")[0];


	m_map_env["AUTH_TYPE"] = "";
	m_map_env["REMOTE_ADDR"] = "";
	m_map_env["REMOTE_HOST"] = ""; 
	m_map_env["REMOTE_IDENT"] = "";
	m_map_env["REMOTE_USER"] = "";
	m_map_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	m_map_env["DOCUMENT_ROOT"] = "./";
	m_map_env["CONTENT_LENGTH"] = "";
	m_map_env["CONTENT_TYPE"] = "";
	m_map_env["PATH_INFO"] = m_path;
	m_map_env["PATH_TRANSLATED"] = m_path;
	m_map_env["QUERY_STRING"] = m_request.url.query;
	m_map_env["REDIRECT_STATUS"] = "200";
	m_map_env["REQUEST_URI"] =  m_path + m_request.url.query;

	if (m_request.method == GET)
		m_map_env["REQUEST_METHOD"] = "GET";
	else if (m_request.method == POST)
	{
		m_map_env["REQUEST_METHOD"] = "POST";
		std::string my_content_length = m_request.header_fields["content-length"]->getValue();
		for (int i = 0; i < (int) my_content_length.size(); i++) {
			if (!isdigit(my_content_length[i]))
				my_content_length.erase(my_content_length.begin() + i);
		}
		m_map_env["CONTENT_LENGTH"] = my_content_length;
		m_map_env["CONTENT_TYPE"] = m_request.header_fields["content-type"]->getValue();
		m_map_env["UPLOAD_DIR"] = m_request.context.getValue(m_request.location, "uploaded_files_dir")[0];
	}
	else
		m_map_env["REQUEST_METHOD"] = "DELETE";

	m_map_env["SCRIPT_FILENAME"] = m_path;
	m_map_env["SCRIPT_NAME"] = m_path;
	m_map_env["SERVER_PORT"] = m_request.context.getValue("serv", "port")[0];
	m_map_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	m_map_env["SERVER_SOFTWARE"] = "Webserv/1.1";
	m_map_env["FILENAME"] = m_request.filename;
	m_map_env["LOCATION"] = m_request.filename;
	m_env = getEnv();
}

char **CGIHandler::getEnv(void)
{
	try
	{
		char **env = (char **)malloc(sizeof(char *) * (m_map_env.size() + 1));
		MapStringIterator it = m_map_env.begin();
		size_t	index = 0;
		for (; it != m_map_env.end(); ++it)
		{
			std::string tmp = it->first + "=" + it->second;
			env[index] = strdup(tmp.c_str());
			index++;
		}
		env[index] = NULL;
		return env;
	}
	catch (std::bad_alloc &e)
	{
		// std::cerr << e.what() << std::endl;
		throw Request::InvalidHttpRequest("internal Server Error, 500 ERROR", InternalServerError);
	}
}

std::string	CGIHandler::getBin(void)
{
	std::string binProg;
	
	size_t		found = m_path.find_last_of('.');
	std::string	extention = m_path.substr(found, m_path.size());
	if (found == std::string::npos)
		throw ExecutionCGIFailedException("invalid extension");
	if (extention == ".php")
		binProg = "/usr/bin/php-cgi";
	else if (extention == ".py")
		binProg = "/usr/bin/python3";
	if (access(binProg.c_str(), F_OK | X_OK) < 0)
		throw ExecutionCGIFailedException("can't find or can't execute the binary");
	return binProg;
}

std::string	CGIHandler::getBodyCGI(void)
{
	return m_body_response;
}

void	timeOutHandler(int signum)
{
	(void)signum;
}

void	CGIHandler::executeCGI(void)
{
	int			saved_in = -1;
	int			saved_out = -1;
	int			pid = -1; 

	try
	{
		m_args[0] = strdup((char *)getBin().c_str());
		m_args[1] = strdup((char *)m_path.c_str());
		m_args[2] = NULL;
		
		saved_in = dup(STDIN_FILENO);
		saved_out = dup(STDOUT_FILENO);
		if (saved_in < 0 || saved_out < 0)
			throw ExecutionCGIFailedException(DUP_ERR);

		FILE	*fIn = std::tmpfile();
		FILE	*fOut = std::tmpfile();
		long	fdIn = fileno(fIn);
		long	fdOut = fileno(fOut);

		const char *tmp = m_body_request.data();
		if (write(fdIn, tmp, m_body_size) < 0)
			throw ExecutionCGIFailedException(WRITE_ERR);
		rewind(fIn);
		pid = fork();
		if (pid < 0)
			throw ExecutionCGIFailedException(FORK_ERR);
		if (pid == 0)
		{
			signal(SIGALRM, timeOutHandler);
			alarm(10);
			if (dup2(fdIn, STDIN_FILENO) < 0)
				throw ExecutionCGIFailedException(DUP2_ERR);
			if (dup2(fdOut, STDOUT_FILENO) < 0)
				throw ExecutionCGIFailedException(DUP2_ERR);
			if (execve(getBin().c_str(), m_args, m_env) < 0)
			{
				// std::cerr << "error execve() " << errno << std::endl;
				throw ExecutionCGIFailedException(EXECVE_ERR);
			}
		}
		else
		{
			char	buffer[1024] = {0};
			int		status;
			waitpid(-1, &status, 0);
			if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM)
				throw ExecutionCGIFailedException(TIMEOUT_ERR);
			lseek(fdOut, 0, SEEK_SET);
			size_t	ret = 1;
			while (ret > 0)
			{
				memset(&buffer, 0, 1024);
				ret = read(fdOut, buffer, 1024 - 1);
				m_body_response += buffer;
				
			}
		}
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		if (saved_in < 0 || saved_out < 0)
			throw ExecutionCGIFailedException(DUP2_ERR);
		if (m_args[0] != NULL)
			free(m_args[0]);
		if (m_args[1] != NULL)
			free(m_args[1]);
		fclose(fIn);
		fclose(fOut);
		close(fdIn);
		close(fdOut);
		close(saved_in);
		close(saved_out);
	}
	catch(const std::exception& e)
	{
		if (saved_in != -1)
			close(saved_in);
		if (saved_out != -1)
			close(saved_out);
		if (m_args[0] != NULL)
			free(m_args[0]);
		if (m_args[1] != NULL)
			free(m_args[1]);
		// std::cerr << e.what() << std::endl;
		throw Request::InvalidHttpRequest("internal Server Error, 500 ERROR", InternalServerError);
	}
}