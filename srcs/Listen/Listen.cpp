#include "Listen.hpp"

Listen::Listen(Config &config)
: _event(0), _revent(0), _config(config) {}

Listen::~Listen() {}

Listen::Listen(const Listen &l) :
_port(l.getPort()), _host(l.getHost()), _event(l.getEvent()),
_revent(l.getRevent()), _config(l._config) {}

Listen	&Listen::operator=(const Listen &src) {
	if (this == &src) { return *this; }
	this->setPort(src.getPort());
	this->setHost(src.getHost());
	this->setEvent(src.getEvent());
	this->setRevent(src.getRevent());
	this->_config = src._config;
	return *this;
}

void				Listen::setPort(const std::string &port) { this->_port = port; }
const std::string	&Listen::getPort() const { return this->_port; }
void				Listen::setHost(const std::string &host) { this->_host = host; }
const std::string	&Listen::getHost() const { return this->_host; }

// void	Listen::setFd(int fd) { this->_fd = fd; }
// int		Listen::getFd() const { return this->_fd; }
void	Listen::setEvent(int event) { this->_event = event; }
int		Listen::getEvent() const { return this->_event; }
void	Listen::setRevent(int revent) { this->_revent = revent; }
int		Listen::getRevent() const { return this->_revent; }

const Config	&Listen::getConfig() const { return this->_config; }
Config	&Listen::getConfig() { return this->_config; }
