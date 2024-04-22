#include "Server.hpp"

static bool	isPortHosts(const std::string &port, const std::string &host, const std::vector<std::pair<std::string, std::string> > &porthosts) {
	for (size_t i = 0; i < porthosts.size(); i += 1) {
		if (porthosts[i].first == port && porthosts[i].second == host) {
			return false;
		}
	}
	return true;
}

static std::vector<std::pair<std::string, std::string> >	porthostsSet(const std::vector<ConfigServer> &servers) {
	std::vector<std::pair<std::string, std::string> >	porthosts;
	std::vector<ConfigServer>::const_iterator it = servers.begin();
	for (; it != servers.end(); ++it) {
		if (isPortHosts(it->port, it->host, porthosts)) {
			std::pair<std::string, std::string> p;
			p.first = it->port;
			p.second = it->host;
			porthosts.push_back(p);
		}
	}
	return porthosts;
}

static int	ftListen(const std::string &port, const std::string &host) {
	struct addrinfo	hints, *result;
	int sock;
	std::memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(host.empty() ? NULL : host.c_str(), port.c_str(), &hints, &result) < 0) {
		errMessage("getaddrinfo");
		return -1;
	}
	sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sock < 0) {
		errMessage("socket");
		freeaddrinfo(result);
		return -1;
	}
	int optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		errMessage("setsockopt");
		freeaddrinfo(result);
		close(sock);
		return -1;
	}
	if (bind(sock, result->ai_addr, result->ai_addrlen) < 0) {
		errMessage("bind");
		freeaddrinfo(result);
		close(sock);
		return -1;
	}
	freeaddrinfo(result);
	if (listen(sock, SOMAXCONN) < 0) {
		errMessage("listen");
		close(sock);
		return -1;
	}
	if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
		errMessage("fcntl");
		close(sock);
		return -1;
	}
	return sock;
}

int	Server::setUp() {
	std::vector<std::pair<std::string, std::string> >	porthosts = porthostsSet(_config.getServers());

	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = porthosts.begin();
	it != porthosts.end(); ++it) {
		int sock = ftListen(it->first, it->second);
		if (sock < 0) {
			return -1;
		}
		std::pair<std::map<int, Listen>::iterator, bool> result = _listens.insert(std::make_pair(sock, Listen(_config)));
		Listen &l = result.first->second;
		l.setPort(it->first);
		l.setHost(it->second);
		l.setEvent(POLLIN);
		std::cout << "listen [" << l.getHost() << ":" << l.getPort() << "]" << std::endl;
	}
	return 0;
}
