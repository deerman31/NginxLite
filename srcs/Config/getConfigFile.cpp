#include "Config.hpp"

static bool isCommentBlank(const std::string& line) {
	if (line.size() == 0) {
		return false;
	}
	size_t i = 0;
	while (line[i] == ' ' || line[i] == '\t') {
		i += 1;
	}
	if (i == line.size()) {
		return false;
	}
	if (line[i] == '#') {
		return false;
	}
	return true;
}

static std::string endTrim(const std::string &s, const char *set) {
	std::string	ret;

	if (s.length() == 0)
		return s;
	size_t l = s.length() -1;
	while (l > 0 && std::strchr(set, s[l]) != NULL) {
		l -= 1;
	}
	if (l == 0 || l == s.length() - 1) {
		return s;
	}
	return s.substr(0, l + 1);
}

static std::string	removeCommentBlank(const std::string &s) {
	std::string			ret;
	std::istringstream stream(s);

	std::string	line;
	while (std::getline(stream, line)) {
		if (isCommentBlank(line)) {
			line = endTrim(line, " 	;");
			ret += line + "\n";
		}
	}
	return ret;
}

static bool	getFile(const std::string &path, std::string &s) {
	std::ifstream		inputFile(path);

	if (!inputFile.is_open()) {
		errMessage("open");
		return false;
	}
	std::string	line;
	s = "";
	while (std::getline(inputFile, line)) {
		s += line + "\n";
	}
	inputFile.close();
	return true;
}

bool	Config::getConfigFile(const std::string &path, std::string &configFile) {
	std::string	file;

	if (!getFile(path, file)) {
		return false;
	}
	file = removeCommentBlank(file);
	configFile = file;
	return true;
}
