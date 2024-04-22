#include "Server.hpp"

int	Server::_connectTimeout() {
	for (std::map<int, Connection>::iterator c = _connects.begin();
	c != _connects.end(); ++c) {
		if (c->second.getConnectionStat() >= RECV && c->second.getConnectionStat() <= SEND) {
			if (60 < std::difftime(std::time(NULL), c->second.getRequestAttemps().first)) {
				c->second.resetAttemps();
			}
			if (CONNECTING_TIMEOUT < std::difftime(std::time(NULL), c->second.getConnectingTime())) {
				c->second.setConnectionStat(CLOSE);
			}
		}
	}
	return 0;
}