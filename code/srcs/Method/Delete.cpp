#include "Server.hpp"

static const std::string	resHtmlDelete(const std::string& num, const std::string& fileName) {
	std::string r;

	r = "<!DOCTYPE html><html><head><title>";
	r += num;
	r += "</title></head><body><center><h1>";
	r += num;
	r += "</h1></center><hr><center>webserv</center></body></html>";
	r += "</h1></center><hr><center>";
	r += fileName;
	r += "</center></body></html>";
	return r;
}

int	Server::_Delete(Connection &c) {
	if (c.getConnectionStat() != DELETE) { return 0; }

	const ConfigServer &servConfig = c.setConfig();
	const Line &line = c.getRequestInfo().getLine();
	const Headers &header = c.getRequestInfo().getHeader();
	std::string path = c.getPath(servConfig);

	Response	resInfo;
	std::string		response;

	int ret = ftStat(path.c_str());
	if (ret == 1) {
		if (std::remove(path.c_str()) != 0) {
			//removeの失敗はリクエストの不備ではなく、サーバー側の不備のため500
			std::cout << "Filed!" << std::endl;
			c.setConnectionStat(ERROR);
			c.setErrStatus(500);
			return 0;
		} else {
			std::cout << "DELETE -> " << path << std::endl;
		}
		resInfo.status = resStatusCode(200);
		response = resHtmlDelete(resInfo.status, path.substr(path.rfind("/")));
		resInfo.contentType = "text/html; charset=UTF-8";
	} else {
		//400 or 403のエラー
		if (ret == 2) {
			ret = 404;
		}
		c.setConnectionStat(ERROR);
		c.setErrStatus(ret);
		return 0;
	}
	resInfo.httpV = line.httpv;
	resInfo.serverName = "webserv";
	resInfo.date = _getRFC1123Date();
	resInfo.contentLength = ftItoa(response.size());
	if (header.connection) {
		resInfo.connection = "keep-alive";
	} else {
		resInfo.connection = "close";
	}
	resInfo.resource = response;

	c.setResponse(resInfo);
	c.setConnectionStat(SEND);
	c.setEvent(POLLOUT);

	return 0;
}
