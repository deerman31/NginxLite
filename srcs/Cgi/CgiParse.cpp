#include "Server.hpp"

int	Server::_cgiResponseCreate(Connection &c) {
	//if (!(c.getConnectionStat() == CGIREAD && c.getCgiRevent() == 0)) {
	if (c.getConnectionStat() != CGIPARSE) {
		return 0;
	}

	// const ConfigServer &servConfig = c.setConfig();
	const std::string tmp(c.getCgiBuf());

	// 読み込みが終わった
	c.clearCgiBuf();
	//読み込みが終わっているため、Cgiのfdをcloseする
	c.closeCgiReadFd();

	std::string	r("\r\n");
	if (tmp.find(r) == std::string::npos) {
		r = "\n";
	}

	// 502 Bad Gateway 空行がないためCGIのレスポンスが不適切と判断し、502を返す
	if (tmp.find(r + r) == std::string::npos) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(502);
		this->_Error(c);
		return 0;
	}


	if (tmp.find("Content-Length: ") != std::string::npos) {
		std::string	length(tmp.substr(tmp.find("Content-Length: ")));
		length = length.substr(0, length.find(r));
		length = length.substr(17);
		size_t len = ftStoi(length);
		length = tmp.substr(tmp.find(r+r) +r.size()+ r.size());
		// スクリプトに書かれているContent-Lengthがレスポンスボディよりも少ないため、502
		if (length.length()<len) {
			c.setConnectionStat(ERROR);
			c.setErrStatus(502);
			this->_Error(c);
			return 0;
		}
	}

	std::string	s;
	const std::string newLine("\r\n");
	s = "HTTP/1.1 200 OK" + newLine;
	if (tmp.find("Content-Length: ") == std::string::npos) {
		size_t	length;
		if (tmp.find("\r\n\r\n") != std::string::npos) {
			length = tmp.substr(tmp.find("\r\n\r\n") + 4).length();
		} else if (tmp.find("\n\n") != std::string::npos) {
			length = tmp.substr(tmp.find("\n\n") + 2).length();
		} else {
			length = tmp.length();
		}
		s += "Content-Length: " + ftItoa(length) + newLine;
	}
	if (tmp.find("Content-Type: ") == std::string::npos) {
		s += "Content-Type: application/octet-stream" + newLine;
	}
	if (tmp.find("Server: ") == std::string::npos) {
		s += "Server: webserv" + newLine;
	}
	if (c.getRequestInfo().getHeader().connection) {
		s += "Connection: keep-alive" + newLine;
	} else {
		s += "Connection: close" + newLine;
	}


	c.setSendMessage(s + tmp);
	c.setEvent(POLLOUT);
	c.setConnectionStat(SEND);
	return 0;
}

int	Server::_CgiParse() {
	std::map<int, Connection>::iterator c = _connects.begin();
	for (; c != _connects.end(); ++c) {
		_cgiResponseCreate(c->second);
	}
	return 0;
}