#include "Config.hpp"

Config::Config() {}
Config::~Config() {}
Config::Config(const Config &src) :
_servers(src.getServers()) {}
Config	&Config::operator=(const Config &src) {
	if (this == &src) { return *this; }
	this->_servers = src.getServers();
	return *this;
}

int	Config::configParse(const std::string &path) {
	std::string					file;

	if (!getConfigFile(path, file)) {
		std::exit(0);
	}
	//configファイルが正しいかどうかチェックする関数をここで適用する。
	if (!isConfig(file)) {
		std::exit(0);
	}

	ConfigServer	s;
	std::vector<std::string> strs = configSplit(file);
	for (std::vector<std::string>::const_iterator it = strs.begin(); it != strs.end(); ++it) {
		std::vector<std::string> strs2 = delSplit(*it, '\n');
		headendTrim(strs2);
		s = servCreate(strs2);
		this->_servers.push_back(s);
	}
	int	count;
	std::vector<ConfigServer>::const_iterator c = _servers.begin();
	for (; c != _servers.end(); ++c) {
		count = 0;
		std::vector<ConfigServer>::const_iterator it = _servers.begin();
		for (; it != _servers.end(); ++it) {
			if (c->port == it->port && c->host == it->host
			&& c->serverName == it->serverName) {
				count += 1;
			}
		}
		if (count != 1) {
			std::cout << "ERROR: The same server name is assigned to the same port and host!" << std::endl;
			std::exit(0);
		}
	}
	defaultServSet();
	return 0;
}


const std::vector<ConfigServer>	&Config::getServers() const {
	return this->_servers;
}