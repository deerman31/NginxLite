#include "Connection.hpp"

const std::string	Connection::getPath(const ConfigServer &servConfig) {
	std::string tmp = getRequestInfo().getLine().path.substr(1);
	if (tmp.find("/") != std::string::npos) {
		tmp = tmp.substr(0, tmp.find("/"));
	}
	tmp = "/" + tmp;
	const Location &l = nowLocation(servConfig);
	std::string path;
	if (l.path == "/") {
		path = l.root + getRequestInfo().getLine().path;
	} else {
		path = l.root + getRequestInfo().getLine().path.substr(tmp.length());
	}
	size_t pos = path.find("?");
	if (pos != std::string::npos) {
		path = path.substr(0, pos);
	}
	return path;
}

const Location		&Connection::nowLocation(const ConfigServer &servConfig) {
	std::string	tmp(getRequestInfo().getLine().path);
	std::vector<Location>::const_iterator it = servConfig.locations.begin();
	for (;it != servConfig.locations.end(); ++it) {
		if (it->path == "/") {
			continue;
		}
		if (tmp.find(it->path) == 0) {
			return *it;
		}
	}
	for (std::vector<Location>::const_iterator it = servConfig.locations.begin();
	it != servConfig.locations.end(); ++it) {
		if ("/" == it->path) {
			return *it;
		}
	}
	return *it;
}

const ConfigServer	&Connection::setConfig() {
	std::vector<ConfigServer>::const_iterator it = getConfig().getServers().begin();
	for (; it != getConfig().getServers().end(); ++it) {
		if (this->getPort() == it->port && this->getHost() == it->host) {
			if (getRequestInfo().getHeader().host.find(":" + this->getPort()) != std::string::npos) {
				if (it->serverName == getRequestInfo().getHeader().host.substr(0, getRequestInfo().getHeader().host.rfind(":"))) {
					return *it;
				}
			} else {
				if (it->serverName == getRequestInfo().getHeader().host) {
					return *it;
				}
			}
		}
	}
	for (it = getConfig().getServers().begin(); it != getConfig().getServers().end(); ++it) {
		if (this->getPort() == it->port && this->getHost() == it->host) {
			if (it->defaultServer) {
				break;
			}
		}
	}
	return *it;
}
