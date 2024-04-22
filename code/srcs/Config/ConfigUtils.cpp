#include "Config.hpp"

static std::string	configSplit2(const std::string& s) {
	size_t start = s.find("{");
	//size_t end = 0;
	size_t a = 1;
	size_t b = 0;
	size_t i;
	for (i = start + 1; a != b; i += 1) {
		if (s[i] == '{') {
			a += 1;
		} else if (s[i] == '}') {
			b += 1;
		}
	}
	return s.substr(0, i);
} 

std::vector<std::string>	Config::configSplit(const std::string& s) {
	std::vector<std::string>	strs;
	std::string ss;
	size_t i = 0;
	while (i < s.size()) {
		ss = configSplit2(&s[i]);
		strs.push_back(ss);
		i += ss.size() + 1;
	}
	return strs;
}

std::vector<std::string>	Config::delSplit(const std::string& s, char del) {
	std::vector<std::string>	strs;
	size_t start = 0;
	size_t end = 0;

	while ((end = s.find(del, start)) != std::string::npos) {
		strs.push_back(s.substr(start, end - start));
		start = end + 1;
	}
	strs.push_back(s.substr(start));
	return strs;
}

static size_t	ftchrlen(const std::string &s, const char *del) {
	size_t	i;

	i = 0;
	while (s[i] && std::strchr(del, s[i]) != NULL) {
		i += 1;
	}
	return i;
}

void	Config::headendTrim(std::vector<std::string> &strs) {
	std::vector<std::string>::iterator it = strs.begin();
	for (; it != strs.end(); ++it) {
		*it = it->substr(ftchrlen(*it, " 	"));
	}
}

void	Config::printConfig() {
	std::vector<ConfigServer>::const_iterator servers = this->_servers.begin();
	for (; servers != this->_servers.end(); ++servers) {
		std::cout << "host -> [" << servers->host << "]\n";
		std::cout << "port -> [" << servers->port << "]\n";
		std::cout << "root -> [" << servers->root << "]\n";
		std::cout << "servername -> [" << servers->serverName << "]\n";
		if (servers->defaultServer == true) {
			std::cout << "defaultServer -> [ON]\n";
		}
		if (!servers->errorPages.empty()) {
			std::map<int, std::string>::const_iterator e = servers->errorPages.begin();
			std::cout << "----error_page----\n";
			for (; e != servers->errorPages.end(); ++e) {
				std::cout << e->first << " -> [" << e->second << "]" << std::endl;
			}
			std::cout << "------------------\n";
		}
		std::cout << "limitBodySize -> [" << servers->limitBodySize << "]\n";
		if (!servers->locations.empty()) {
			std::cout << "----Locations----\n";
			std::vector<Location>::const_iterator l = servers->locations.begin();
			for (; l != servers->locations.end(); ++l) {
				std::cout << "path -> [" << l->path << "]\n";
				if (!l->allowMethods.empty()) {
					std::cout << "----allowMethods----\n";
					std::vector<std::string>::const_iterator m = l->allowMethods.begin();
					for (; m != l->allowMethods.end(); ++m) {
						std::cout << *m << std::endl;
					}
					std::cout << "-----------------\n";
				}
				if (!l->redirect.empty()) {
					std::cout << "redirect -> " << l->redirect << std::endl;
				}
				if (!l->root.empty()) {
					std::cout << "root -> [" << l->root << "]\n";
				}
				if (l->autoIndex) {
					std::cout << "autoindex -> [ON]\n";
				}
				if (!l->indexFiles.empty()) {
					std::cout << "----indexFiles----\n";
					std::vector<std::string>::const_iterator i = l->indexFiles.begin();
					for (; i != l->indexFiles.end(); ++i) {
						std::cout << *i << std::endl;
					}
					std::cout << "-----------------\n";
				}
				if (!l->cgiHandler.empty()) {
					std::cout << "----cgi----\n";
					std::vector<std::string>::const_iterator c = l->cgiHandler.begin();
					for (; c != l->cgiHandler.end(); ++c) {
						std::cout << *c << std::endl;
					}
					std::cout << "-----------------\n";
				}
				if (!l->uploadPath.empty()) {
					std::cout << "upload -> [" << l->uploadPath << "]\n";
				}
			}
			std::cout << "-----------------\n";
		}
	}
}
