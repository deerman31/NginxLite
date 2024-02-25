#include "Server.hpp"

int	Server::_CgiWrite(Connection &c) {
	if (c.getConnectionStat() != CGIWRITE) {
		return 0;
	}

	std::string	tmp(c.getCgiWriteBody());
	std::string	s(tmp.substr(0, BUF_SIZE));
	ssize_t ret = write(c.getCgiWriteFd(), s.c_str(), s.length());
	if (ret < 0) {
		// 500 リクエストには問題なく、単純なwriteによるエラー、要するにサーバー自体のエラーのため500
		errMessage("write");
		c.setConnectionStat(CLOSE);
		return 0;
	} else if (ret == 0) {
		std::cout << "write 接続が切れた -> [" << c.getCgiWriteFd() << "]" << std::endl;
		c.closeCgiWriteFd();
		c.clearCgiWriteBody();
		c.setConnectionStat(CLOSE);
		return 0;
	}
	std::cout << "CGI write [" << c.getCgiWriteFd() << "] " << ret << "bytes" << std::endl;
	if (tmp != s) {
		c.setCgiWriteBody(tmp.substr(ret));
		return 0;
	}

	c.closeCgiWriteFd();
	c.clearCgiWriteBody();

	c.setCgiTime(std::time(NULL));
	c.setConnectionStat(CGIWAIT);
	return 0;
}