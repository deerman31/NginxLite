#ifndef LISTEN_HPP
#define LISTEN_HPP

#include "Config.hpp"

#include <string>
#include <cstdlib>

#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

class Listen {
	private:
	std::string		_port;
	std::string		_host;

	int				_event;
	int				_revent;

	Config	&_config;

	public:
	Listen(Config &config);
	Listen(const Listen &);
	Listen	&operator=(const Listen &);
	~Listen();
	void	setPort(const std::string &);
	void	setHost(const std::string &);
	const std::string	&getPort() const;
	const std::string	&getHost() const;

	void	setEvent(int);
	void	setRevent(int);
	int	getEvent() const;
	int	getRevent() const;

	const Config	&getConfig() const;
	Config	&getConfig();
};

void	errMessage(const std::string &s);

#endif
