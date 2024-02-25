#include "Server.hpp"

static bool	isClose(const Connection &c) {
	if (!c.getKeep()) {
		return true;
	}
	int n = c.getErrStatus();
	if (n == 400 || n == 401 || n == 403 || n == 404 || n == 408 || n == 429 ||  n == 500 || n == 501 || n == 502 || n == 503 || n == 504 || n == 505) {
		return true;
	}
	return false;
}

static std::string	generate(const Response &info) {
	const std::string r("\r\n");
	std::string	response;

	response = info.httpV + " " + info.status + r;
	response += "Server: " + info.serverName + r;
	if (!info.date.empty()) {
		response += "Date: " + info.date + r;
	}
	if (!info.contentType.empty()) {
		response += "Content-Type: " + info.contentType + r;
	}
	if (!info.contentLength.empty()) {
		response += "Content-Length: " + info.contentLength + r;
	}
	if (!info.lastModified.empty()) {
		response += "Last-Modified: " + info.lastModified + r;
	}
	if (!info.location.empty()) {
		response += "Location: " + info.location + r;
	}
	response += "Connection: " + info.connection + r;
	response += r;
	response += info.resource;
	return response;
}

int	Server::_Send(int fd, Connection &c) {
	c.setConnectingTime(std::time(NULL));

	if (c.getSendMessage().empty()) {
		c.setSendMessage(generate(c.getResponse()));
	}
	std::string tmp(c.getSendMessage());
	std::string	s;

	s = tmp.substr(0, BUF_SIZE);

	ssize_t ret = send(fd, s.c_str(), s.length(), 0);
	if (ret < 0) {
		errMessage("send");
		c.setConnectionStat(CLOSE);
		return 0;
	} else if (ret == 0) {
		std::cout << "send 接続が切れた -> " << fd << " フラグをCLOSEに設定" << std::endl;
		c.setConnectionStat(CLOSE);
		return 0;
	}

	std::cout << "send [" << fd << "] " << ret << "bytes" << std::endl;
	if (tmp != s) {
		c.setSendMessage(tmp.substr(ret));
		return 0;
	}

	if (isClose(c) || _connects.size() + _listens.size() > MAX_CLIENT) {
		c.setConnectionStat(CLOSE);
	} else {
		c.setEvent(POLLIN);
		c.setConnectionStat(RECV);
	}

	c.setPid(0);
	c.setErrStatus(0);
	c.clearRequestInfo();
	c.clearResponse();
	c.clearSendMessage();
	c.clearCgiBuf();
	c.clearCgiWriteBody();

	return 0;
}