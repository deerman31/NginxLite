#include "Server.hpp"

int	Server::_NewAccept(int fd, const Listen &l) {
	if (!(l.getRevent() & POLLIN)) {
		return 0;
	}

	sockaddr_in	addr;
	socklen_t	addrLen = sizeof(addr);
	int			sock;

	std::memset(&addr, 0, sizeof(addr));

	sock = accept(fd, (struct sockaddr*)&addr, &addrLen);
	if (sock < 0) {
		errMessage("accept");
		return -1;
	}

	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		errMessage("setsockopt");
		close(sock);
		return -1;
	}
	if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
		errMessage("fcntl");
		close(sock);
		return -1;
	}

	std::pair<std::map<int, Connection>::iterator, bool> result = _connects.insert(std::make_pair(sock, Connection(_config)));
	Connection	&c = result.first->second;

	c.setPort(l.getPort());
	c.setHost(l.getHost());
	c.setConnectingTime(std::time(NULL));
	c.setConnectionStat(RECV);
	c.setEvent(POLLIN);
	c.setRevent(0);
	c.setCgiReadFd(-1);
	std::pair<time_t, size_t> pair(std::time(NULL), 0);
	c.setRequestAttemps(pair);

	std::cout << "New: listen [" << fd << "] -> connection [" << sock << "]" << std::endl;
	return 0;
}