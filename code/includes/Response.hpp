#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>

struct Response {
	std::string		httpV;
	std::string		date;
	std::string		status;
	std::string		serverName;
	std::string		contentType;
	std::string		contentLength;
	std::string		lastModified;
	std::string		location;
	std::string		connection;
	std::string		resource;
};

#endif