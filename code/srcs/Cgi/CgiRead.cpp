#include "Server.hpp"

int	Server::_CgiRead(Connection &c) {
	if (c.getConnectionStat() != CGIREAD) {
		return 0;
	}

	// 408 Request Timeout readに一定時間以上かかる場合はrecvと同じく408を返す
	if (TIMEOUT408 < difftime(std::time(NULL), c.getReadTime())) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(408);
		return 0;
	}

	char	buf[BUF_SIZE + 1];
	std::memset(buf, 0, sizeof(buf));
	ssize_t ret = read(c.getCgiReadFd(), buf, BUF_SIZE);
	if (ret < 0) {
		errMessage("read");
		c.setConnectionStat(CLOSE);
		return 0;
	} else if (ret == 0) {
		c.setConnectionStat(CGIPARSE);
		std::cout << "read 接続が切れた -> [" << c.getCgiReadFd() << "]" << std::endl;
		//c.setConnectionStat(CLOSE);
		return 0;
	}
	std::string b(buf, ret);
	std::cout << "CGI read [" << c.getCgiReadFd() << "] " << ret << "bytes" << std::endl;
	c.addCgiBuf(b);
	return 0;
}