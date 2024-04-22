#include "webserv.hpp"

const std::string	resStatusCode(int statusNum) {
	if (statusNum == 100) { return "100 Continue"; }
	else if (statusNum == 101) { return "101 Switching Protocols"; }
	else if (statusNum == 200) { return "200 OK"; }
	else if (statusNum == 201) { return "201 Created"; }
	else if (statusNum == 204) { return "204 No Content"; }
	else if (statusNum == 301) { return "301 Moved Permanently"; }
	else if (statusNum == 400) { return "400 Bad Request"; }
	else if (statusNum == 403) { return "403 Forbidden"; }
	else if (statusNum == 404) { return "404 Not Found"; }
	else if (statusNum == 405) { return "405 Method Not Allowed"; }
	else if (statusNum == 408) { return "408 Request Timeout"; }
	else if (statusNum == 411) { return "411 Length Required"; }
	else if (statusNum == 413) { return "413 Payload Too Large"; }
	else if (statusNum == 414) { return "414 URI Too Long"; }
	else if (statusNum == 415) { return "415 Unsupported Media Type"; }
	else if (statusNum == 429) { return "429 Too Many Requests"; }
	else if (statusNum == 431) { return "431 Request Header Fields Too Large"; }
	else if (statusNum == 500) { return "500 Internal Server Error"; }
	else if (statusNum == 501) { return "501 Not Implemented"; }
	else if (statusNum == 502) { return "502 Bad Gateway"; }
	else if (statusNum == 504) { return "504 Gateway Timeout"; }
	else if (statusNum == 505) { return "505 HTTP Version Not Supported"; }
	return "";
}

static const std::string	errorHtml(int errNum) {
	const std::string	num = resStatusCode(errNum);
	std::string			r;
	r = "<!DOCTYPE html><html><head><title>";
	r += num;
	r += "</title></head><body><center><h1>";
	r += num;
	r += "</h1></center><hr><center>webserv</center></body></html>";
	return r;
}

static int	isDefaultError(int errorCode, const ConfigServer &servConfig) {
	if (servConfig.errorPages.empty()) {
		return 0;
	}
	if (servConfig.errorPages.find(errorCode) == servConfig.errorPages.end()) {
		return 0;
	}
	std::map<int, std::string> tmp = servConfig.errorPages;
	std::string p = tmp[errorCode];

	int n = ftStat(p.c_str());
	return n;
}

std::string	Server::_defaultErrorHtml(Connection &c, int errorCode, const ConfigServer &servConfig) {
	const std::string a("\r\n");
	std::map<int, std::string> tmp = servConfig.errorPages;
	std::string p = tmp[errorCode];
	//std::string resource = _readFileContents(p);
	std::string resource;
	if (!_readFileContents(resource, p)) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(500);
		return 0;
	}
	std::string	res;
	res = "HTTP/1.1 ";
	res += resStatusCode(errorCode) + a;
	res += "Server: " + servConfig.serverName + a;
	res += "Date: " + _getRFC1123Date() + a;
	res += "Content-Type: " + setContentsType(p) + a;
	res += "Content-Length: " + ftItoa(resource.length()) + a;
	res += "Connection: ";
	//if (!c.getConnection() || errorCode == 400 || errorCode == 503) {
	if (!c.getKeep() || errorCode == 400 || errorCode == 503) {
		res += "close" + a;
	} else {
		res += "keep-alive" + a;
	}
	res += a;
	res += resource;

	return res;
}

int	Server::_Error(Connection &c) {
	if (c.getConnectionStat() != ERROR) { return 0; }

	const ConfigServer &servConfig = c.setConfig();

	int ret = isDefaultError(c.getErrStatus(), servConfig);
	if (ret == 1) {
		c.setSendMessage(_defaultErrorHtml(c, c.getErrStatus(), servConfig));
		c.setEvent(POLLOUT);
		return 0;
	} else if (ret != 0) {
		// retが1でないということは設定されているdefaultのエラーページがない、権限がない、directoryのどれか
		// 権限がない場合は403、ない場合は404、あるがdirectoryである場合にも404
		if (ret == 2) {
			ret = 404;
		}
		c.setErrStatus(ret);
	}

	const std::string html(errorHtml(c.getErrStatus()));
	const std::string r("\r\n");
	std::string response("HTTP/1.1 ");
	response += resStatusCode(c.getErrStatus()) + r;
	response += "Server: webserv" + r;
	response += "Date: " + _getRFC1123Date() + r;
	response += "Content-Type: text/html" + r;
	response += "Content-Length: " + ftItoa(html.length()) + r;
	if (c.getErrStatus() == 405) {
		const Location &l = c.nowLocation(servConfig);
		response += "Allow: ";
		for (size_t i = 0; i < l.allowMethods.size(); i += 1) {
			if (i != 0) {
				response += ", ";
			}
			response += l.allowMethods[i];
		}
		response += r;
	}
	response += "Connection: ";
	int n = c.getErrStatus();
	if (!c.getKeep() || n == 400 || n == 401 || n == 403 || n == 404 || n == 408 || n == 429 ||  n == 500 || n == 501 || n == 502 || n == 503 || n == 504) {
		response += "close" + r;
	} else {
		response += "keep-alive" + r;
	}
	response += r;
	response += html;

	c.setSendMessage(response);
	c.clearRequestInfo();
	c.clearResponse();


	c.setEvent(POLLOUT);
	c.setConnectionStat(SEND);

	return 0;
}