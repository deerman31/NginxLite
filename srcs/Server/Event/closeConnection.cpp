#include "Server.hpp"

int	Server::_closeConnection() {
	for (std::map<int, Connection>::iterator c = _connects.begin();
	c != _connects.end();) {
		if (c->second.getConnectionStat() == CLOSE) {
			std::cout << "Connection close [" << c->first << "]" << std::endl;
			close(c->first);
			c->second.closeCgiReadFd();
			c->second.closeCgiWriteFd();
			c->second.killPid();
			c = _connects.erase(c);
		} else {
			++c;
		}
	}
	return 0;
}