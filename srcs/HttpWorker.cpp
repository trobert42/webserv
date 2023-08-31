/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpWorker.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trobert <trobert@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/21 16:16:13 by agras             #+#    #+#             */
/*   Updated: 2023/08/08 19:46:59 by trobert          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <cstring>

#include "../includes/HttpWorker.hpp"
#include "../includes/ResponseBuilder.hpp"
#include "../includes/HttpWorker.hpp"

HttpWorker::HttpWorker() : http_tables(HttpTables::getInstance())
{
}

HttpWorker::~HttpWorker()
{
} 

e_req_method	getMethodFromString(std::string str)
{
	if (str == "GET")
		return GET;
	else if (str == "POST")
		return POST;
	else if (str == "DELETE")
		return DELETE;
	else
		throw Request::InvalidHttpRequest("bad method token in http request, METHOD NOT ALLOWED 405 ERROR", MethodNotAllowed);
	return GET;
}

s_http_version	getVersionFromString(std::string str)
{
	s_http_version version;
	if (str == "HTTP/0.9")
	{
		version.major = 0;
		version.minor = 9;
	}
	else if (str == "HTTP/1.0")
	{
		version.major = 1;
		version.minor = 0;
	}
	else if (str == "HTTP/1.1")
	{
		version.major = 1;
		version.minor = 1;
	}
	else if (str == "HTTP/2.0")
	{
		version.major = 2;
		version.minor = 0;
	}
	else if (str == "HTTP/3.0")
	{
		version.major = 3;
		version.minor = 0;
	}
	else
		throw Request::InvalidHttpRequest("unknown protocol version in http request, VERSION NOT SUPPORTED 505 ERROR", HttpVersionNotSupported);
	return version;
}

s_url	HttpWorker::parseUrl(std::string str)
{
	s_url	url;
	std::string	*looking_for = &url.authority;

	std::size_t begin = str.find("://");
	if (begin != std::string::npos)
	{
		url.scheme = str.substr(0, begin);
		str = str.substr(begin + 3, std::string::npos);
	}

	begin = str.find(':');
	if (begin != std::string::npos)
	{
		url.authority = str.substr(0, begin);
		str = str.substr(begin + 1, std::string::npos);
		looking_for = &url.port;
	}

	begin = str.find('/');
	if (begin != std::string::npos)
	{
		*looking_for = str.substr(0, begin);
		str = str.substr(begin, std::string::npos);
		looking_for = &url.path;
	}

	begin = str.find('?');
	if (begin != std::string::npos)
	{
		*looking_for = str.substr(0, begin);
		str = str.substr(begin + 1, std::string::npos);
		looking_for = &url.query;
	}
	*looking_for = str;

	return url;
}

unsigned char	getValue(const unsigned char c)
{
	if (c >= 'A')
		return (c - 'A' + 10);
	return (c - '0');
}

std::string	HttpWorker::decodeUrlEncodedChars(std::string encoded_str)
{
	std::string	decoded_str;
	for (std::size_t i = 0; i < encoded_str.size();)
	{
		if (encoded_str[i] == '%' && i + 2 <= encoded_str.size())
		{
			unsigned char aasci_value = 0;
			if (++i <= encoded_str.size())
				aasci_value = getValue(encoded_str[i]) * 16;
			if (++i <= encoded_str.size())
				aasci_value += getValue(encoded_str[i]);
			decoded_str += aasci_value;
		}
		else
			decoded_str += encoded_str[i];
		i++;
	}
	return (decoded_str);
}

std::string	getRequestLocation(Request &request, ServerContext &context)
{
	if (!request.url.path.empty() && request.url.path[request.url.path.size() - 1] != '/')
	{
		std::string tmp = request.url.path;
		tmp += '/';
		if (opendir((context.getValue("serv", "root")[0] + tmp).c_str()) != NULL)
		{
			if (context.isExistingLocat(tmp))
			{
				request.url.path += "/";
				return request.url.path;
			}
		}
	}
	if (context.isExistingLocat(request.url.path))
		return request.url.path;
	return "serv";
}

void	HttpWorker::parseStatusLine(Request& request, std::string line)
{
	size_t begin = 0;
	size_t pos = line.find(' ', begin);
	request.method = getMethodFromString(line.substr(begin, pos - begin));

	for (; pos < line.size() && line[pos] == ' '; pos++);

	begin = pos;
	pos = line.find(' ', begin);
	request.url = parseUrl(line.substr(begin, pos - begin));
	request.url.scheme = decodeUrlEncodedChars(request.url.scheme);
	request.url.authority = decodeUrlEncodedChars(request.url.authority);
	request.url.path = decodeUrlEncodedChars(request.url.path);
	request.url.query = decodeUrlEncodedChars(request.url.query);

	for (; pos < line.size() && line[pos] == ' '; pos++);

	begin = pos;
	pos = line.find(' ', begin);
	request.version = getVersionFromString(line.substr(begin, pos - begin));
	if (request.version.major != 1 || request.version.minor != 1)
		throw (Request::InvalidHttpRequest("http version not supported, HTTP VERSION NOT SUPPORTED 505 ERROR", HttpVersionNotSupported));
}

std::string	VectorToString(std::vector<char>::iterator begin, std::vector<char>::iterator end)
{
	std::string str;
	for (; begin != end; begin++)
		str.push_back(*begin);
	return str;
}

void	HttpWorker::parseRequest(Request& request, Connection& connection)
{
	std::vector<char>	line_sep;
	line_sep.push_back('\r');
	line_sep.push_back('\n');
	while (true)
	{
		std::vector<char>::iterator it = findRangeinRange(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, connection.read_buffer.data.end(), line_sep.begin(), line_sep.end());
		if (it == connection.read_buffer.data.end())
			return;
		std::string line = VectorToString(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, it);
		connection.read_buffer.head_pos = it - connection.read_buffer.data.begin() + 2;
		if (line.empty())
			break;
		if (request.version.major == 0 && request.version.minor == 0)
			parseStatusLine(request, line);
		else
		{
			int sep = line.find(':', 0);
			std::string field_name = tolowerString(line.substr(0, sep));
			if (request.header_fields.count(field_name) > 0)
				throw Request::InvalidHttpRequest("field duplicate in request header, BAD REQUEST 400 ERROR", BadRequest);
			if (isSameHeader(field_name, "content-length"))
				request.header_fields[field_name] = new ContentLength(trimFrontSpaces(line.substr(sep + 1, std::string::npos)));
			else
				request.header_fields[field_name] = new GenericHeader(field_name, trimFrontSpaces(line.substr(sep + 1, std::string::npos)));

		}
	}
	request.state = READING_BODY;
}

int	HttpWorker::retrieveChunk(Request& request, Connection& connection)
{
	std::vector<char>	line_sep;
	line_sep.push_back('\r');
	line_sep.push_back('\n');
	if (request.chunk_size == 0)
	{
		std::vector<char>::iterator it = findRangeinRange(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, connection.read_buffer.data.end(), line_sep.begin(), line_sep.end());
		if (it == connection.read_buffer.data.end())  // waiting to receive more bytes
			return -1;
		std::string line = VectorToString(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, it);
		connection.read_buffer.head_pos = it - connection.read_buffer.data.begin() + 2;
		request.chunk_size = strtoul(line.c_str(), NULL, 16);
		if (request.chunk_size == 0)  // last chunk identified -> server received entire chunked body
		{
			request.state = READY;
			request.header_fields["content-length"] = new ContentLength(request.body.size());
			return 0;
		}
	}

	std::vector<char>::iterator	chunk_end;
	chunk_end = findRangeinRangeLimit(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, connection.read_buffer.data.end(), line_sep.begin(), line_sep.end(), request.chunk_size + 2);
	if (chunk_end == connection.read_buffer.data.end())
		return -1;

	request.chunk = std::vector<char>(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, chunk_end);
	for (size_t i = 0; i < request.chunk.size(); i++)
		request.body.push_back(request.chunk[i]);
	connection.read_buffer.head_pos = chunk_end - connection.read_buffer.data.begin() + 2;
	request.chunk_size = 0;
	return 1;
}

void	HttpWorker::retrieveRequestBody(Request& request, Connection& connection)
{
	if (request.method == POST || request.method == DELETE)
	{
		if (request.header_fields.count("content-length") > 0)
		{
			size_t content_length = atoi(request.header_fields.find("content-length")->second->getValue().c_str());
			if (content_length > static_cast<size_t>(atoi(request.context.getValue(request.location, "client_max_body_size")[0].c_str())))
				throw Request::InvalidHttpRequest("request body is too long, PAYLOAD TOO LARGE 413 ERROR", PayloadTooLarge);
			std::vector<char>::iterator	range_end = connection.read_buffer.data.begin() + connection.read_buffer.head_pos + content_length;
			if (range_end > connection.read_buffer.data.end())
				range_end = connection.read_buffer.data.end();
			std::vector<char> new_bytes(connection.read_buffer.data.begin() + connection.read_buffer.head_pos, range_end);
			request.body = new_bytes;
			if (request.body.size() < content_length)
				return;
			connection.read_buffer.head_pos += new_bytes.size();
		}
		else if (request.header_fields.count("transfer-encoding") > 0)
		{
			while (retrieveChunk(request, connection) == 1);
			return;
		}
	}
	request.state = READY;
}

bool	HttpWorker::isCGI(std::string path)
{
	std::string extension = extractExtension(path);
	if (http_tables->isCGIsupported(extension))
		return true;
	return false;
}

int	HttpWorker::checkErrorPageDirective(Request &request, e_status_code status, std::string& body)
{
	VectorString to_use = request.context.getValue("serv", "error_page");

	if (request.location != "serv")
	{
		VectorString locat_error_page = request.context.getValue(request.location, "error_page");
		if (!locat_error_page.empty())
			to_use = locat_error_page;
	}
	if (to_use.empty())
		return 0;
	for (VectorIterator it = to_use.begin(); it != to_use.end(); ++it)
	{
		if (strtof((*it).c_str(), NULL) == status)
		{
			std::string path = request.context.getValue("serv", "root")[0] + (*++it);
			if (isFileReadable(path) == false)
			{
				response_builder.reset();
				response_builder.setProtocol("HTTP/1.1");
				response_builder.setBody(body);
				response_builder.setStatus(status);
				return -1;
			}
			response_builder.setBodyFromFile(path);
			response_builder.setStatus(status);
			return -1;
		}
	}
	return 0;
}

void	HttpWorker::makeResponse(Request& request, e_status_code status, std::string body)
{
	response_builder.reset();
	response_builder.setProtocol("HTTP/1.1");
	if (status >= BadRequest && status <= NetworkAuthenticationRequired)
	{
		if (checkErrorPageDirective(request, status, body) < 0)
			return ;
	}
	response_builder.setBody(body);
	response_builder.setStatus(status);
}

void	HttpWorker::makeResponseFromFile(e_status_code status, std::string path)
{
	response_builder.reset();
	response_builder.setProtocol("HTTP/1.1");
	response_builder.setBodyFromFile(path);
	response_builder.setStatus(status);
}

void	HttpWorker::handleRequest(Connection& connection, std::string ip)
{
	if (connection.requests.empty() || connection.requests.back().state >= READY)
		connection.createNewRequest();
	try
	{
		if (connection.getNextRequest().state == READING_HEADER)
			parseRequest(connection.getNextRequest(), connection);
		if (connection.getNextRequest().state > READING_HEADER && !connection.getNextRequest().isAttributedToAServer())
		{
			connection.getNextRequest().context = connection.getSpecificContext(ip);
			connection.getNextRequest().location = getRequestLocation(connection.getNextRequest(), connection.getNextRequest().context);
			connection.getNextRequest().is_server_attributed = true;
			isMethodValid(connection.getNextRequest());
		}
		if (connection.getNextRequest().state == READING_BODY)
			retrieveRequestBody(connection.getNextRequest(), connection);
		if (connection.getNextRequest().state == READY)
		{
			std::cout << YELLOW << "--------------- REQUEST ---------------" << std::endl;
			connection.getNextRequest().printRequestInfo();
			std::cout << "--------------------------------------\n" << WHITE << std::endl;
			serveRequest(connection.getNextRequest());
		}
	}
	catch (const Request::InvalidHttpRequest& e)
	{
		std::cerr << RED << "CATCHED ERROR : " << e.what() << WHITE << std::endl;
		connection.read_buffer.head_pos = connection.read_buffer.data.size();
 		connection.getNextRequest().state = RESPONSE_READY;
		makeResponse(connection.getNextRequest(), e.getStatus(), e.what());
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "CATCHED ERROR : " << e.what() << WHITE << std::endl;
		connection.read_buffer.head_pos = connection.read_buffer.data.size();
		connection.getNextRequest().state = RESPONSE_READY;
		makeResponse(connection.getNextRequest(), InternalServerError, e.what());
	}
	if (connection.getNextRequest().state == RESPONSE_READY)
		processResponse(connection.getNextRequest(), connection);
	connection.received_new_bytes = false;
}

void	HttpWorker::serveRequest(Request &request)
{
	if (request.location != "serv" && !request.context.getValue(request.location, "rewrite").empty())
	{
		std::cout << GREY << "-rewrite" << WHITE << std::endl;
		makeResponse(request, static_cast<e_status_code>(atoi(request.context.getValue(request.location, "rewrite")[0].c_str())), "");
		response_builder.addHeaderField(new Location(request.context.getValue(request.location, "rewrite")[1]));
		request.state = RESPONSE_READY;
		return;
	}
	std::string path = request.context.getValue(request.location, "root")[0] + request.url.path;
	std::cout << GREY
		<< "-Inside serverRequest" << std::endl
		<< "  block server used n" << request.context.getBlockIndex()
		<< " [" << request.context.getValue("serv", "ip address")[0] << ":" << request.context.getPort() << "]" << std::endl
		<< "  request.location = [" << request.location << "]" << std::endl
		<< "  path = [" << path << "]" << WHITE << std::endl << std::endl;
	if (isFileExisting(path) == false)
	{
		std::cout << GREY << "-no existing path = [" << path << "]" << WHITE << std::endl;
		throw Request::InvalidHttpRequest("what's that page? NOT FOUND 404 ERROR", NotFound);
	}
	if (isFileInaccessible(path))
	{
		std::cout << GREY << "-can't access path = [" << path << "]" << WHITE << std::endl;
		throw Request::InvalidHttpRequest("not authorized, FORBIDDEN 403 ERROR", Forbidden);
	}
	if (isCGI(path) && request.context.getValue(request.location, "cgi")[0] == "off")
		throw Request::InvalidHttpRequest("cgi's off, BAD GATEWAY 502 ERROR", BadGateway);
	if (request.method == GET)
		processGet(request);
	else if (request.method == DELETE)
		processDelete(request);
	else if (request.method == POST)
		processPost(request);
	request.state = RESPONSE_READY;
	std::cout << std::endl;
}

std::string	makeCGIBody(ResponseBuilder	&response_builder, Request *request, std::string &path, std::string &env)
{
	CGIHandler			cgi_handler(*request, path, env);
	std::stringstream	cgi_output;
	std::string 		line;

	cgi_output << cgi_handler.getBodyCGI();
	while (std::getline(cgi_output, line) && line != "\r")
	{
		int sep = line.find(':');
		response_builder.addHeaderField(new GenericHeader(line.substr(0, sep), line.substr(sep + 1, std::string::npos)));
	}

	std::string remaining;
	if (!cgi_output.eof()) {
		std::ostringstream remainingStream;
		remainingStream << cgi_output.rdbuf();
		remaining = remainingStream.str();
	}
	return remaining;
}

void	HttpWorker::processResponse(Request& request, Connection& connection)
{
	if (request.state != RESPONSE_READY)
		return;
	connection.read_buffer.data.erase(connection.read_buffer.data.begin(), connection.read_buffer.data.begin() + connection.read_buffer.head_pos);
	connection.read_buffer.head_pos = 0;
	request.response = response_builder.harvestResponse();
	std::cout << GREEN << "--------------- RESPONSE --------------- \n"
		<< request.response.getVersion() << " status: " << request.response.getStatus() << std::endl;
	for (size_t index = 0; index < request.response.getHeadersCount(); index++)
		std::cout << request.response.getHeader(index)->getName() << ": " << request.response.getHeader(index)->getValue() << std::endl;
	std::cout << "---------------------------------------" << WHITE << std::endl;
	writeResponseToBuffer(connection, request.response);
	connection.removeRequest();
}



void	HttpWorker::processGet(Request &request)
{
	std::string path = request.context.getValue(request.location, "root")[0] + request.url.path;
	std::cout << GREY
			<< "-Inside processGet() with location [" << request.location << "]" << std::endl
			<< "  request.url.path = [" << request.url.path << "]"<< std::endl
			<< "  complete root+path = [" << path << "]" << WHITE << std::endl;
	if (!path.empty() && path.find_last_of('/') == path.size() - 1)
	{
		std::cout << GREY << "	-URL finishing by /" << WHITE << std::endl;
		if (!request.context.getValue(request.location, "index").empty())
		{
			path += request.context.getValue(request.location, "index")[0];
			std::cout << GREY << "		-Index not empty" << std::endl
				<< "		  path = [" << path << "]" << WHITE << std::endl;
			if (isFileExisting(path) == false)
				throw Request::InvalidHttpRequest("what's that page? NOT FOUND 404 ERROR", NotFound);
			if (isFileInaccessible(path))
				throw Request::InvalidHttpRequest("not authorized, FORBIDDEN 403 ERROR", Forbidden);
			makeResponseFromFile(Ok, path);
		}
		else if (request.context.getValue(request.location, "autoindex")[0] == "on" && request.context.getValue(request.location, "index").empty())
		{
			if (request.context.getValue(request.location,"cgi")[0] == "off")
			{
				std::cout << GREY << "		-Index empty, autoindex off -> no page" << WHITE << std::endl;
				throw Request::InvalidHttpRequest("cgi's off, BAD GATEWAY 502 ERROR", BadGateway);
			}
			request.filename =  "../" + path;
			std::cout << GREY << "		-Index empty & autoindex on --> autoindex cgi" << std::endl
				<< "		Route = [" << request.filename << "]" << WHITE << std::endl;
			std::string script_path = "html/autoindex.php";
			makeResponse(request, Ok, makeCGIBody(response_builder, &request, script_path, request.filename));
		}
		else
		{
			std::cout << GREY << "		-Index empty, autoindex off -> no page" << WHITE << std::endl;
			makeResponse(request, Forbidden, "FORBIDDEN, 403 ERROR");
		}
	}
	else if (isCGI(request.url.path))
	{
		std::cout << GREY << "	-Url finishing by php --> cgi execution" << WHITE << std::endl;
		makeResponse(request, Ok, makeCGIBody(response_builder, &request, path, request.location));
	}
	else
	{
		std::cout << GREY << "	-Retrieving normal Get method" << WHITE << std::endl;
		makeResponseFromFile(Ok, path);
	}
}

void	HttpWorker::handlePostMultipart(Request &request)
{
	size_t 		pos_boundary = request.header_fields["content-type"]->getValue().find("boundary=") + 9;
	std::string	boundary = "--" + request.header_fields["content-type"]->getValue().substr(pos_boundary);
	std::string	request_body = vectorCharToString(request.body);
	std::size_t	pos = 0;

	while (true)
	{
		std::size_t start = request_body.find(boundary, pos);
		if (start == std::string::npos)
			break;

		std::size_t end = request_body.find(boundary, start + boundary.length());
		if (end == std::string::npos)
			break;

		std::string part = request_body.substr(start + boundary.length(), end - start - boundary.length());

		std::size_t header_end = part.find("\r\n\r\n");
		std::string header = part.substr(0, header_end);
		std::string data = part.substr(header_end + 4);

		std::size_t name_start = header.find("name=\"") + 6;
		std::size_t name_end = header.find("\"", name_start);
		std::string field_name = header.substr(name_start, name_end - name_start);

		if (header.find("filename=\"") != std::string::npos)
		{
			std::size_t filename_start = header.find("filename=\"") + 10;
			std::size_t filename_end = header.find("\"", filename_start);
			std::string filename = header.substr(filename_start, filename_end - filename_start);

			std::fstream file(filename.c_str(), std::ios::out | std::ios::binary);
			if (file.is_open())
			{
				file.write(data.data(), data.size() - 2);
				file.close();
				std::string uploaded_dir_name = request.context.getValue(request.location, "root")[0] + request.context.getValue(request.location, "uploaded_files_dir")[0];
				std::string old_path_file = "./" + filename;
				std::string new_path_file = uploaded_dir_name + filename;
				request.filename = filename;
				if (isFileReadable(uploaded_dir_name) == false || std::rename(old_path_file.c_str(), new_path_file.c_str()) < 0)
				{
					unlink(old_path_file.c_str());
					throw Request::InvalidHttpRequest("internal Server Error, 500 ERROR", InternalServerError);
				}
			}
		}
		else
		{
			std::cerr << "Form field: " << field_name << ", Value: " << data << std::endl;
		}
		pos = end + boundary.length();
	}
}


void	HttpWorker::processPost(Request &request)
{
	std::cout << GREY << "-Inside processPost() with location [" << request.location << "]" << WHITE << std::endl;
	response_builder.reset();
	response_builder.setProtocol("HTTP/1.1");
	std::string path = request.context.getValue(request.location, "root")[0] + request.url.path;
	if (request.header_fields.count("content-type") < 1)
		throw Request::InvalidHttpRequest("no Content-Type header, BAD REQUEST 400 ERROR", BadRequest);
	std::string content_type = request.header_fields["content-type"]->getValue();
	std::cout << GREY << " content-type: " << content_type << WHITE << std::endl;
	if (!http_tables->getMIMETypeExtensions(content_type).empty())
		throw Request::InvalidHttpRequest("invalid value for Content-Type header, UNSUPPORTED MEDIA TYPE 415 ERROR", UnsupportedMediaType);
	if (content_type.find("multipart/form-data") != std::string::npos)
		handlePostMultipart(request);
	if (request.header_fields.count("content-length") > 0 && isCGI(path))
		makeResponse(request, Created, makeCGIBody(response_builder, &request, path, request.location));
	else
		makeResponse(request, Created, vectorCharToString(request.body));
}

void	HttpWorker::processDelete(Request &request)
{
	response_builder.reset();
	response_builder.setProtocol("HTTP/1.1");
	std::string path = request.context.getValue(request.location, "root")[0] + request.url.path;
	if (std::remove(path.c_str()) == 0)
		response_builder.setStatus(NoContent);
	else if (isFileExisting(path))
		throw Request::InvalidHttpRequest("not authorized to remove a restricted file or a directory, FORBIDDEN 403 ERROR", Forbidden);
	else
		throw Request::InvalidHttpRequest("what's that page? NOT FOUND 404 ERROR", NotFound);
}

void	HttpWorker::respondError(Connection& connection, Request request, const Request::InvalidHttpRequest& exception)
{
	makeResponse(request, exception.getStatus(), exception.what());
	request.state = RESPONSE_READY;
	processResponse(request, connection);
}

void	HttpWorker::writeResponseToBuffer(Connection& connection, Response& response)
{
	std::string	response_buffer;
	std::stringstream	stream;

	stream << response.getVersion();
	std::string	version;
	stream >> version;
	response_buffer += version;
	response_buffer += " ";

	std::stringstream	stream2;
	unsigned int code = response.getStatus();
	stream2 << code;
	std::string	code_as_str;
	stream2 >> code_as_str;
	response_buffer += code_as_str;
	response_buffer += " ";

	response_buffer += http_tables->getStatusDescription(response.getStatus());
	response_buffer += "\r\n";

	for (std::size_t i = 0; i < response.getHeadersCount(); i++)
	{
		response_buffer += response.getHeader(i)->getName() + ": "
		+ response.getHeader(i)->getValue() + "\r\n";
	}

	response_buffer += "\r\n";
	response_buffer += response.getBody();

	for (size_t i = 0; i < response_buffer.size(); i++)
		connection.write_buffer.data.push_back(response_buffer[i]);
}

bool	HttpWorker::isMethodValid(Request& request) const
{
	const VectorString context_methods = request.context.getValue(request.location, "allowed_methods");
	if (std::find(context_methods.begin(), context_methods.end(), stringify_method(request.method)) == context_methods.end())
		throw (Request::InvalidHttpRequest("method not allowed at this location, METHOD NOT ALLOWED 405 ERROR", MethodNotAllowed));
	return true;
}

bool	HttpWorker::isSameHeader(std::string a, std::string b)
{
	if (a.size() != b.size())
		return false;
	for (size_t i = 0; i < a.size(); i++)
	{
		if (std::tolower(a[i] != std::tolower(b[i])))
			return false;
	}
	return true;
}

std::istream	&operator>>(std::istream &in, s_http_version &rhs)
{
	std::string	buffer;

	in >> buffer;
	if (buffer == "HTTP/0.9")
	{
		rhs.major = 0;
		rhs.minor = 9;
	}
	else if (buffer == "HTTP/1.0")
	{
		rhs.major = 1;
		rhs.minor = 0;
	}
	else if (buffer == "HTTP/1.1")
	{
		rhs.major = 1;
		rhs.minor = 1;
	}
	else if (buffer == "HTTP/2.0")
	{
		rhs.major = 2;
		rhs.minor = 0;
	}
	else if (buffer == "HTTP/3.0")
	{
		rhs.major = 3;
		rhs.minor = 0;
	}
	else
		throw Request::InvalidHttpRequest("unknown protocol version in http request, VERSION NOT SUPPORTED 505 ERROR", HttpVersionNotSupported);
	return in;
}

std::ostream	&operator<<(std::ostream &out, const s_http_version &rhs)
{
	out << "HTTP/" << rhs.major << '.' << rhs.minor;
	return out;
}

std::istream	&operator>>(std::istream &in, e_req_method &rhs)
{
	std::string	buffer;
	in >> buffer;
	if (buffer == "GET")
		rhs = GET;
	else if (buffer == "POST")
		rhs = POST;
	else if (buffer == "DELETE")
		rhs = DELETE;
	else
		throw Request::InvalidHttpRequest("bad method token in http request, METHOD NOT ALLOWED 405 ERROR", MethodNotAllowed);
	return in;
}

std::ostream	&operator<<(std::ostream &out, const e_req_method &rhs)
{
	if (rhs == GET)
		out << "GET";
	else if (rhs == POST)
		out << "POST";
	else if (rhs == DELETE)
		out << "DELETE";
	return out;
}

std::string		stringify_method(const e_req_method &rhs)
{
	if (rhs == GET)
		return "GET";
	else if (rhs == POST)
		return "POST";
	else if (rhs == DELETE)
		return "DELETE";
	return "";
}

std::ostream	&operator<<(std::ostream &out, const s_url &rhs)
{
	out << "scheme=\"" << rhs.scheme << "\" authority=\"" << rhs.authority << "\" port=\"" << rhs.port << "\" path=\"" << rhs.path << "\" query=\"" << rhs.query << "\"";
	return out;
}

std::ostream	&operator<<(std::ostream &out, Request &request)
{
	out << request.method << ' ' << request.url << ' ' << request.version << '\n';

	std::map<std::string, HeaderField*>::iterator it = request.header_fields.begin();
	for (; it != request.header_fields.end(); it++)
		out << it->second->getName() << ':' << it->second->getValue() << '\n';

	return out;
}
