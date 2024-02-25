#include "Config.hpp"

static bool bracesCheck(const std::string &str) {
	std::stack<int> braces;
	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == '{') {
			// '{' の前に "server " または "location /" があるかをチェック
			bool isValid = false;
			if (i > 6 && str.substr(i - 7, 7) == "server ") {
				isValid = true;
			} else if (i > 10) { // "location /" のために少なくとも10文字必要
				size_t locationPos = str.rfind("location ", i - 1);
				if (locationPos != std::string::npos && locationPos + 9 < i && str[locationPos + 9] == '/') {
					// "location" の後に '/' があり、その後にパスが続いているか確認
					isValid = str[i - 1] == ' ';
				}
			}
			if (isValid) {
				braces.push(i);
			} else {
				return false; // 無効な '{' の配置
			}
		} else if (str[i] == '}') {
			if (braces.empty()) {
				return false; // 対応する '{' がない
			}
			braces.pop();
		}
	}
	return braces.empty(); // スタックが空なら正しく対応している
}

static bool areLocationsNestedInServers(const std::string &str) {
	std::vector<std::pair<size_t, size_t> > serverBlocks; // 各serverブロックの(開始位置, 終了位置)を保持
	std::stack<size_t> openBraces; // 開いた中括弧の位置を追跡

	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] == '{') {
			openBraces.push(i);
		} else if (str[i] == '}') {
			if (openBraces.empty()) {
				return false; // 不一致の中括弧
			}
			int openPos = openBraces.top();
			openBraces.pop();
			// 最後に開いた中括弧がserverブロックのものか確認
			if (i > 6 && str.substr(openPos - 7, 7) == "server ") {
				serverBlocks.push_back(std::make_pair(openPos, i));
			}
		}
	}
	if (!openBraces.empty()) {
		return false; // 閉じられていない中括弧がある
	}
	for (size_t i = 0; i < str.size(); ++i) {
		if (str.substr(i, 9) == "location " && str[i + 9] == '/') {
			// locationブロックの開始位置を見つける
			size_t openBracePos = str.find('{', i);
			if (openBracePos == std::string::npos) {
				continue; // '{' が見つからない場合はスキップ
			}
			// 対応する閉じ中括弧を見つける
			size_t closeBracePos = str.find('}', openBracePos);
			if (closeBracePos == std::string::npos) {
				return false; // 閉じ中括弧が見つからない
			}
			// このlocationブロックがどのserverブロックにも収まっていないか確認
			bool isNested = false;
			for (std::vector<std::pair<size_t, size_t> >::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
				if (openBracePos > it->first && closeBracePos < it->second) {
					isNested = true;
					break;
				}
			}
			if (!isNested) {
				return false; // このlocationブロックはどのserverブロックにも収まっていない
			}
		}
	}
	return true; // すべてのlocationブロックがserverブロックに収まっている
}

static void trimEdges(std::string &s, const std::string &d) {
	size_t start = s.find_first_not_of(d);
	if (start == std::string::npos) {
		s.clear();
		return;
	}
	size_t end = s.find_last_not_of(d);
	s = s.substr(start, end - start + 1);
}
static void	vecTrim(std::vector<std::string> &lines, const std::string &d) {
	std::vector<std::string>::iterator it = lines.begin();
	for (; it != lines.end(); ++it) {
		trimEdges(*it, d);
	}
}
static void removeEmptyLines(std::vector<std::string> &lines) {
	std::vector<std::string>::iterator it = lines.begin();
	while (it != lines.end()) {
		if (it->empty()) {
			it = lines.erase(it);
		} else {
			++it;
		}
	}
}
static void removeComments(std::string& s) {
	size_t startPos = 0;
	while ((startPos = s.find('#', startPos)) != std::string::npos) {
		size_t endPos = s.find('\n', startPos);
		if (endPos == std::string::npos) {
			// コメントが行の最後まで続く場合
			s.erase(startPos);
			break; // 文字列の末尾に達したのでループを抜ける
		} else {
			s.erase(startPos, endPos - startPos);
		}
	}
}
static bool	elementNumCheck(const std::vector<std::string> &lines) {
	std::vector<ServerElement>	servers;
	ServerElement				server;
	LocationElement				location;
	int		mode = 0; // 1がserver内の要素のチェック、2がlocation内の要素のチェック

	std::vector<std::string>::const_iterator it = lines.begin();
	while (it != lines.end()) {
		if (it->find("server {") == 0) {
			mode = 1;
			std::memset(&server, 0, sizeof(server));
			servers.push_back(server);
		} else if (it->find("location /") == 0) {
			mode = 2;
			std::memset(&location, 0, sizeof(location));
			servers.back().locaElements.push_back(location);
		} else if (it->find("}") == 0) {
			mode = 0;
		} else if (mode == 1) {
			if (it->find("port ") == 0) {
				servers.back().port += 1;
			} else if (it->find("host ") == 0) {
				servers.back().host += 1;
			} else if (it->find("root ") == 0) {
				servers.back().root += 1;
			} else if (it->find("server_name ") == 0) {
				servers.back().serverName += 1;
			} else if (it->find("error_page ") == 0) {
				servers.back().errorPage += 1;
			} else if (it->find("client_max_body_size ") == 0) {
				servers.back().bodySize += 1;
			} else {
				std::cout << "ERROR: " << *it << " is inappropriate as an element of the server block!" << std::endl;
				return false;
			}
		} else if (mode == 2) {
			if (it->find("root ") == 0) {
				servers.back().locaElements.back().root += 1;
			} else if (it->find("index ") == 0) {
				servers.back().locaElements.back().index += 1;
			} else if (it->find("autoindex ") == 0) {
				servers.back().locaElements.back().autoindex += 1;
			} else if (it->find("cgi ") == 0) {
				servers.back().locaElements.back().cgi += 1;
			} else if (it->find("allow_methods ") == 0) {
				servers.back().locaElements.back().allowmethods += 1;
			} else if (it->find("redirect ") == 0) {
				servers.back().locaElements.back().redirect += 1;
			} else if (it->find("upload ") == 0) {
				servers.back().locaElements.back().upload += 1;
			} else {
				std::cout << "ERROR: " << *it << " is inappropriate as an element of the location block!" << std::endl;
				return false;
			}
		}
		++it;
	}

	std::vector<ServerElement>::const_iterator ss = servers.begin();
	for (; ss != servers.end(); ++ss) {
		if (ss->port != 1) {
			std::cout << "ERROR: The number of ports is 1" << std::endl;
			return false;
		}
		if (ss->host > 1) {
			std::cout << "ERROR: The number of hosts is 0 or 1" << std::endl;
			return false;
		}
		if (ss->root != 1) {
			std::cout << "ERROR: The number of roots is 1" << std::endl;
			return false;
		}
		if (ss->serverName != 1) {
			std::cout << "ERROR: The number of server_name is 1" << std::endl;
			return false;
		}
		if (ss->bodySize != 1) {
			std::cout << "ERROR: The number of client_max_body_size is 1" << std::endl;
			return false;
		}
		std::vector<LocationElement>::const_iterator l = ss->locaElements.begin();
		for (; l != ss->locaElements.end(); ++l) {
			if (l->root > 1) {
				std::cout << "ERROR: The number of roots is 0 or 1" << std::endl;
				return false;
			}
			if (l->index > 1) {
				std::cout << "ERROR: The number of indexs is 0 or 1" << std::endl;
				return false;
			}
			if (l->autoindex > 1) {
				std::cout << "ERROR: The number of autoindexs is 0 or 1" << std::endl;
				return false;
			}
			if (l->cgi > 1) {
				std::cout << "ERROR: The number of cgi is 0 or 1" << std::endl;
				return false;
			}
			if (l->allowmethods > 1) {
				std::cout << "ERROR: The number of allow_methods is 0 or 1" << std::endl;
				return false;
			}
			if (l->redirect > 1) {
				std::cout << "ERROR: The number of redirect is 0 or 1" << std::endl;
				return false;
			}
			if (l->upload > 1) {
				std::cout << "ERROR: The number of upload is 0 or 1" << std::endl;
				return false;
			}
		}
	}
	return true;
}

static bool	isNum(const std::string &s) {
	for (size_t i = 0; i < s.length(); i += 1) {
		if (std::isdigit(s[i]) == 0)
			return false;
	}
	return true;
}

static size_t ft_stoi(const std::string &s) {
	size_t n;
	std::istringstream iss(s);
	iss >> n;
	return n;
}

// IPアドレスの形式をチェックする
static bool isIpAddress(const std::string& ip) {
	std::istringstream ss(ip);
	std::string token;
	int count = 0;
	while (std::getline(ss, token, '.')) {
		int num = ft_stoi(token);
		if (num < 0 || num > 255 || (token.size() > 1 && token[0] == '0'))
			return false;
		++count;
	}
	return count == 4;  // IPv4アドレスは4つの数値で構成される
}

// Portの妥当性をチェックする関数
static bool isPort(int port) {
	return port > 0 && port <= 65535;
}

static bool	elementStrCheck(const std::vector<std::string> &lines) {
	std::vector<std::string>	strs;
	std::vector<std::string>::const_iterator line = lines.begin();
	for (; line != lines.end(); ++line) {
		strs = ftSplit(*line, " ");
		if (line->find("port ") == 0) {
			if (strs.size() != 2 || !isNum(strs[1]) || !isPort(ft_stoi(strs[1]))) {
				std::cout << "ERROR: Inappropriate port value!" << std::endl;
				return false;
			}
		} else if (line->find("host ") == 0) {
			if (strs.size() != 2 || !isIpAddress(strs[1])) {
				std::cout << "ERROR: Inappropriate host value!" << std::endl;
				return false;
			}
		} else if (line->find("root ") == 0) {
			if (strs.size() != 2 || ftStat(strs[1].c_str()) != 2) {
				std::cout << "ERROR: Inappropriate root value!" << std::endl;
				return false;
			}
		} else if (line->find("server_name ") == 0) {
			if (strs.size() != 2) {
				std::cout << "ERROR: Inappropriate server_name value!" << std::endl;
				return false;
			}
		} else if (line->find("error_page ") == 0) {
			if (strs.size() != 3) {
				std::cout << "ERROR: Inappropriate error_page value!" << std::endl;
				return false;
			}
			if (!isNum(strs[1]) || strs[1].length() != 3) {
				std::cout << "ERROR: Inappropriate error_page value!" << std::endl;
				return false;
			}
			int n = ft_stoi(strs[1].c_str());
			if (!(n == 400 || n == 403 || n == 404 || n == 405 ||
			n == 408 || n == 411 || n == 413 || n == 414 || n == 415 ||
			n == 429 || n == 431 || n == 500 || n == 501 ||
			n == 502 || n == 504 || n == 505)) {
				std::cout << "ERROR: Inappropriate error_page value!" << std::endl;
				return false;
			}
			if (ftStat(strs[2].c_str()) != 1) {
				std::cout << "ERROR: Inappropriate error_page value!" << std::endl;
				return false;
			}
		} else if (line->find("client_max_body_size ") == 0) {
			if (!(strs[1].back() == 'M' || strs[1].back() == 'K' ||
			strs[1].back() == 'G' || std::isdigit(strs[1].back()) == 1)) {
				std::cout << "ERROR: Inappropriate client_max_body_size value!" << std::endl;
				return false;
			}
			if (!isNum(strs[1].substr(0, strs[1].length() - 1))) {
				std::cout << "ERROR: Inappropriate client_max_body_size value!" << std::endl;
				return false;
			}
		} else if (line->find("index ") == 0) {
			if (strs.size() == 1) {
				std::cout << "ERROR: Inappropriate index value!" << std::endl;
				return false;
			}
		} else if (line->find("autoindex ") == 0) {
			if (strs.size() != 2) {
				std::cout << "ERROR: Inappropriate autoindex value!" << std::endl;
				return false;
			}
			if (!(strs[1] == "on" || strs[1] == "off")) {
				std::cout << "ERROR: Inappropriate autoindex value!" << std::endl;
				return false;
			}
		} else if (line->find("cgi ") == 0) {
			if (strs.size() > 6 || strs.size() == 1) {
				std::cout << "ERROR: Inappropriate cgi value!" << std::endl;
				return false;
			}
			for (size_t i = 1; i < strs.size(); i += 1) {
				if (!(strs[i] == "cgi" || strs[i] == "py" || strs[i] == "rb" || strs[i] == "pl" || strs[i] == "sh")) {
					std::cout << "ERROR: Inappropriate cgi value!" << std::endl;
					return false;
				}
			}
		} else if (line->find("allow_methods ") == 0) {
			if (strs.size() == 1 || strs.size() > 4) {
				std::cout << "ERROR: Inappropriate allow_methods value!" << std::endl;
				return false;
			}
			for (size_t i = 1; i < strs.size(); i += 1) {
				if (!(strs[i] == "GET" || strs[i] == "POST" || strs[i] == "DELETE")) {
					std::cout << "ERROR: Inappropriate allow_methods value!" << std::endl;
					return false;
				}
			}
		} else if (line->find("redirect ") == 0) {
			if (strs.size() != 2) {
				std::cout << "ERROR: Inappropriate redirect value!" << std::endl;
				return false;
			}
		} else if (line->find("upload ") == 0) {
			if (strs.size() != 2 || ftStat(strs[1].c_str()) != 2) {
				std::cout << strs[1] << std::endl;
				std::cout << "ERROR: Inappropriate upload value!" << std::endl;
				return false;
			}
		}
	}
	return true;
}

bool	Config::isConfig(const std::string &file) {
	// fileの中身が空だとfalse
	if (file.empty()) {
		return false;
	}
	std::string	tmp(file);
	// fileの中の#で始まるコメントを削除
	removeComments(tmp);
	// 中括弧がちゃんと閉じられているか、locationの中括弧がserverの中括弧の中にあるか
	if (!bracesCheck(tmp)) {
		std::cout << "ERROR: Curly braces are not closed." << std::endl;
		return false;
	}
	if (!areLocationsNestedInServers(tmp)) {
		std::cout << "ERROR: The location block is not inside the server block." << std::endl;
		return false;
	}
	std::vector<std::string> lines = ftSplit(tmp, "\n");
	//linesにある空行を削除
	removeEmptyLines(lines);
	//linesの各要素の先頭と末尾にあるspaceとtabをtrimする
	vecTrim(lines, " 	");
	// serverとlocationの各要素をチェック
	if (!elementNumCheck(lines) || !elementStrCheck(lines))
		return false;
	return true;
}