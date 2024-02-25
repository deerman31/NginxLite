#include "Config.hpp"

static int ft_stoi(const std::string &s) {
	int n;
	std::istringstream iss(s);
	iss >> n;
	return n;
}

void Config::errorPageCreate(std::map<int, std::string> &map, const std::string &line) {
	std::vector<std::string> strs = delSplit(line, ' ');
	int n = ft_stoi(strs[0]);
	map[n] = strs[1];
}

static size_t limitBodySizeCreate(const std::string &line) {
	/* バイト単位：client_max_body_size 500
	などと指定すると、サイズはバイト単位で設定されます。
	キロバイト（K）：client_max_body_size 10K は 10 キロバイトを意味します。
	メガバイト（M）：client_max_body_size 10M は 10
	メガバイトを意味します。これは非常に一般的な使用法です。
	ギガバイト（G）：client_max_body_size 2G は 2
	ギガバイトを意味します。これは大きなファイルのアップロードを許可する場合に使用されます。*/
	size_t x = 1;
	if (line.find("K") != std::string::npos) {
		x = 1024;
	} else if (line.find("M") != std::string::npos) {
		x = 1048576;
	} else if (line.find("G") != std::string::npos) {
		x = 1073741824;
	}
	return x * ft_stoi(line);
}

static void trimEndUsingPopBack(std::string &str) {
	while (!str.empty()) {
		char lastChar = str.back();
		if (lastChar == '{' || lastChar == ' ') {
			str.pop_back();
		} else {
			break; // 末尾の文字が削除対象でない場合、ループを抜ける
		}
	}
}

Location Config::locationCreate(const std::vector<std::string> &strs, const std::string &root) {
	Location l;
	std::memset(&l, 0, sizeof(l));

	std::vector<std::string>::const_iterator it = strs.begin();
	l.path = it->substr(9);
	trimEndUsingPopBack(l.path);
	if (l.path.length() != 1 && l.path.back() == '/') {
		l.path.resize(l.path.length() - 1);
	}
	++it;

	l.autoIndex = false;
	for (; it != strs.end(); ++it) {
		if (it->find(_methos) == 0) {
			l.allowMethods = delSplit(it->substr(_methos.length()), ' ');
		} else if (it->find(_redirect) == 0) {
			l.redirect = it->substr(_redirect.length());
		} else if (it->find(_root) == 0) {
			l.root = it->substr(_root.length());
		} else if (it->find(_autoindex) == 0) {
			l.autoIndex = true;
		} else if (it->find(_index) == 0) {
			l.indexFiles = delSplit(it->substr(_index.length()), ' ');
		} else if (it->find(_cgi) == 0) {
			l.cgiHandler = delSplit(it->substr(_cgi.length()), ' ');
		} else if (it->find(_upload) == 0) {
			l.uploadPath = it->substr(_upload.length());
		}
	}
	if (l.root.empty()) {
		if (root.back() == '/') {
			l.root = root.substr(0, root.rfind("/")) + l.path;
		} else {
			l.root = root + l.path;
		}
	}
	if (l.uploadPath.empty()) {
		l.uploadPath = l.root;
	}
	return l;
}

ConfigServer	Config::servCreate(const std::vector<std::string> &strs) {
	std::vector<std::string> ss;
	ConfigServer s;

	s.defaultServer = false;
	for (std::vector<std::string>::const_iterator it = strs.begin(); it != strs.end(); ++it) {
		if (it->find(_root) == 0) {
			s.root = it->substr(_root.length());
		} else if (it->find(_host) == 0) {
			s.host = it->substr(_host.length());
		} else if (it->find(_port) == 0) {
			s.port = it->substr(_port.length());
		} else if (it->find(_servname) == 0) {
			s.serverName = it->substr(_servname.length());
		} else if (it->find(_errorpage) == 0) {
			errorPageCreate(s.errorPages, it->substr(_errorpage.length()));
		} else if (it->find(_client_max_body_size) == 0) {
			s.limitBodySize = limitBodySizeCreate(it->substr(_client_max_body_size.length()));
		} else if (it->find(_location) == 0) {
			if (s.root.empty()) {
				s.root = "./";
			}
			while (it->find("}") == std::string::npos) {
				ss.push_back(*it);
				++it;
			}
			s.locations.push_back(locationCreate(ss, s.root));
			ss.clear();
		}
	}
	bool f = false;
	for (std::vector<Location>::iterator it = s.locations.begin(); it != s.locations.end(); ++it) {
		if (it->path == "/") {
			f = true;
			break;
		}
	}
	if (f == false) {
		Location	l;
		l.path = "/";
		l.root = s.root;
		l.uploadPath = s.root;
		l.autoIndex = false;
		s.locations.push_back(l);
	}
	return s;
}