#include "Config.hpp"

static bool	isDefault(const std::string &port, const std::string &host, const std::vector<std::pair<std::string, std::string> > &porthosts) {
	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = porthosts.begin(); 
	it != porthosts.end(); ++it) {
		if (port == it->first && host == it->second) {
			return false;
		}
	}
	return true;
}

void	Config::defaultServSet() {
	std::vector<std::pair<std::string, std::string> > porthosts;
	for (std::vector<ConfigServer>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it) {
		if (isDefault(it->port, it->host, porthosts)) {
			it->defaultServer = true;
			std::pair<std::string, std::string> p;
			p.first = it->port;
			p.second = it->host;
			porthosts.push_back(p);
		}
	}
}