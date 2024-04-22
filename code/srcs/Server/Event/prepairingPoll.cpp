#include "Server.hpp"

int	Server::_prepairingPoll() {
	_fds.clear();
	pollfd	poll;

	for (std::map<int, Listen>::iterator l = _listens.begin();
	l != _listens.end(); ++l) {
		_fds.push_back(poll);
		_fds.back().fd = l->first;
		_fds.back().events = l->second.getEvent();
	}
	for (std::map<int, Connection>::iterator c = _connects.begin();
	c != _connects.end(); ++c) {
		_fds.push_back(poll);
		_fds.back().fd = c->first;
		_fds.back().events = c->second.getEvent();
		if (c->second.getConnectionStat() == CGIREAD) {
			_fds.push_back(poll);
			_fds.back().fd = c->second.getCgiReadFd();
			_fds.back().events = c->second.getCgiReadEvent();
		} else if (c->second.getConnectionStat() == CGIWRITE) {
			_fds.push_back(poll);
			_fds.back().fd = c->second.getCgiWriteFd();
			_fds.back().events = c->second.getCgiWriteEvent();
		} 
	}
	return 0;
}
