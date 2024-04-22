#include "Server.hpp"

Server::Server(Config &config): _config(config) {}
Server::~Server() {
	std::map<int, Connection>::iterator c = _connects.begin();
	for (; c != _connects.end(); ++c) {
		std::cout << "Connection close -> [" << c->first << "]" << std::endl;
		close(c->first);
		c->second.closeCgiReadFd();
		c->second.closeCgiWriteFd();
	}
	std::map<int, Listen>::iterator l = _listens.begin();
	for (; l != _listens.end(); ++l) {
		std::cout << "Listen [" << l->second.getHost() << ":" << l->second.getPort() << "] close -> [" << l->first << "]" << std::endl;
		close(l->first);
	}
}

bool Server::_readFileContents(std::string &s, const std::string& filePath) {
	std::ifstream inputFile(filePath);
	if (!inputFile.is_open()) {
		std::cout << filePath << std::endl;
		errMessage("open");
		return false;
		std::exit(1);
	}
	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	s = buffer.str();
	return true;
}

std::string	Server::_getLastModified(const std::string& path) {
	struct stat st;
	if (stat(path.c_str(), &st) != 0) {
		errMessage("stat");
		std::exit(1);
	}
	char buf[80];
	struct tm* timeinfo = gmtime(&st.st_mtime);
	strftime(buf, 80, "Last-Modified: %a, %d %b %Y %H:%M:%S GMT", timeinfo);
	return std::string(buf);
}

std::string Server::_getRFC1123Date() {
	std::time_t now = std::time(NULL); // 現在の時刻を取得
	std::tm* gmt = std::gmtime(&now);     // GMT（UTC）に変換

	char buf[100];
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);
	return std::string(buf);
}

void Server::_replaceAll(std::string& str, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = str.find(search, pos)) != std::string::npos) {
		str.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	while (str.find("//") != std::string::npos) {
		size_t n = str.find("//");
		str.erase(str.begin() + n);
	}
	if (*str.begin() != '/') {
		str = "/" + str;
	}
}

std::string Server::_listDirContents(const std::string& path, const Location &location) {
	DIR* dir;
	struct dirent* ent;
	std::string result = "<html><body><h1>Directory Listing</h1><ul>";

	if ((dir = opendir(path.c_str())) != NULL) {

		while ((ent = readdir(dir)) != NULL) {
			std::string fname = ent->d_name;
			std::string fullPath = path + fname;
			int n = ftStat(fullPath.c_str());
			if (n == 2 && path == (location.root.back() == '/' ? location.root : location.root + "/")) {
				if (fname == "." || fname == "..") {
					continue;
				}
			}
			// Create a clickable link
			std::string href = n == 2 ? fullPath + "/" : fullPath;
			if (!location.root.empty()) {
				_replaceAll(href, location.root, location.path);
			}

			result += "<li><a href=\"" + href + "\">" + fname + (n==2 ? "/" : "") + "</a></li>";
		}
		closedir(dir);
	} else {
		// Could not open directory
		result += "<p>Directory could not be opened.</p>";
	}
	result += "</ul></body></html>";
	return result;
}

bool	Server::_isMediaType(const std::string &path) {
	const std::string ex = path.substr(path.rfind(".") + 1);
	const std::string	medias[100] = {"asf", "acd", "asx", "au", "avi",
	"aif", "dig", "iff", "lso", "mid", "midi", "mov", "mp3", "mp4", "mpg",
	"msf", "qt", "ra", "ram", "rm", "rpm", "sd", "sdn", "svx", "vqe", "vqf",
	"vql", "wav", "wma", "wrk", "3gp", "3g2", "aac", "aiff", "aif", "asf",
	"asnd", "avi", "bwf", "crm", "dv", "gif", "helf", "hevc", "m1v", "m2t",
	"m2ts", "m2v", "m4a", "m4v", "mov", "mpeg", "mpe", "mpg", "mts", "mxf",
	"r3d", "rush", "wob", "wav", "NULL"};
	for (size_t i = 0; medias[i] != "NULL"; i += 1) {
		if (medias[i] == ex) {
			return true;
		}
	}
	return false;
}