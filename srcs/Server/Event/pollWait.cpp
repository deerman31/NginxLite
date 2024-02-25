#include "Server.hpp"

int	Server::_pollWait() {
	int		ret;

	ret = poll(_fds.data(), _fds.size(), 0);
	if (ret < 0) {
		if (errno == EINTR) {
			return -1;
		}
		errMessage("poll");
		return -1;
	} else if (ret == 0) {
		std::cout << "Wait..." << std::endl;
	}
	this->_updateSocketStatus(ret);
	return ret;
}

int	Server::_updateSocketStatus(int count) {
	if (count == 0) { return 0;}
	std::vector<pollfd>::iterator f = _fds.begin();
	for (std::map<int, Listen>::iterator l = _listens.begin(); l != _listens.end(); ++l) {
		l->second.setRevent(f->revents);
		++f;
	}
	for (std::map<int, Connection>::iterator c = _connects.begin();c != _connects.end(); ++c) {
		c->second.setRevent(f->revents);
		if (c->second.getConnectionStat() == CGIREAD) {
			++f;
			c->second.setCgiReadRevent(f->revents);
		} else if (c->second.getConnectionStat() == CGIWRITE) {
			++f;
			c->second.setCgiWriteRevent(f->revents);
		} 
		++f;
	}
	return 0;
}