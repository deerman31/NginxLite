#include "Server.hpp"

static bool	allowMethodCheck(Connection &c, const ConfigServer &servConfig) {
	std::string	method;;
	ConnectionStatus status = c.getConnectionStat();
	if (status == GET) {
		method = "GET";
	} else if (status == POST) {
		method = "POST";
	} else if (status == DELETE) {
		method = "DELETE";
	}

	const Location &location = c.nowLocation(servConfig);

	if (!location.allowMethods.empty()) {
		std::vector<std::string>::const_iterator it = location.allowMethods.begin();
		for (; it != location.allowMethods.end(); ++it) {
			if (*it == method) {
				break;
			}
		}
		if (it == location.allowMethods.end()) {
			c.setConnectionStat(ERROR);
			return false;
		}
	}
	return true;
}

static bool	isCgi(Connection &c, const ConfigServer &servConfig, const Line &line) {
	if (line.method == "DELETE") {
		return false;
	}
	const std::vector<std::string> &cgis = c.nowLocation(servConfig).cgiHandler;
	if (cgis.empty()) {
		return false;
	}
	std::string path;
	size_t pos = line.path.find("?");
	if (pos != std::string::npos) {
		path = line.path.substr(0, pos);
	} else {
		path = line.path;
	}
	pos = path.rfind(".");
	if (pos == std::string::npos) {
		return false;
	}
	const std::string extension = path.substr(pos + 1);
	for (std::vector<std::string>::const_iterator it = cgis.begin();
	it != cgis.end(); ++it) {
		if (extension == *it) {
			return true;
		}
	}
	return false;
}

static bool	isRedirect(Connection &c, const ConfigServer &servConfig) {
	const Location &location = c.nowLocation(servConfig);

	std::string	tmp;
	if (c.getRequestInfo().getLine().path.back() == '/' && c.getRequestInfo().getLine().path.length() != 1) {
		tmp = c.getRequestInfo().getLine().path.substr(0, c.getRequestInfo().getLine().path.length() - 1);
	} else {
		tmp = c.getRequestInfo().getLine().path;
	}
	if (location.path != tmp) {
		return false;
	}
	if (location.redirect.empty()) {
		return false;
	}
	return true;
}

int	Server::_Method(Connection &c) {
	ConnectionStatus status = c.getConnectionStat();
	if (!(status == GET || status == POST || status == DELETE)) {
		return 0;
	}
	const ConfigServer &servConfig = c.setConfig();

	// configのserver内のlocationに設定しているmethod以外がリクエストされている場合には405
	if (!allowMethodCheck(c, servConfig)) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(405);
		return 0;
	}

	// configのserver内のlocationに設定しているbodyのサイズをオーバーしている場合には413
	if (servConfig.limitBodySize != 0 && servConfig.limitBodySize < c.getRequestInfo().getBody().body.size()) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(413);
		return 0;
	}
	if (isRedirect(c, servConfig)) {
		c.setConnectionStat(REDIRECT);
		return 0;
	}
	if (isCgi(c, servConfig, c.getRequestInfo().getLine())) {
		c.setConnectionStat(CGIEXEC);
		return 0;
	}
	_Get(c);
	_Post(c);
	_Delete(c);
	return 0;
}