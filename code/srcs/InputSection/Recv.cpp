#include "Server.hpp"

static bool	vectorSearch(const std::vector<std::string> tokens, const std::string &str) {
	for (std::vector<std::string>::const_iterator it = tokens.begin(); it != tokens.end(); ++it) {
		if (it->find(str) == 0) { return true; }
	}
	return false;
}

static bool	chunkCheck(const std::string& buf, const std::vector<std::string> tokens) {

	if (!vectorSearch(tokens, "Transfer-Encoding: chunked")) { return true; }

	const std::string tra = "Transfer-Encoding: ";
	if (!vectorSearch(tokens, tra)) { return true; }

	std::string tmp = buf.substr(buf.find(tra) + tra.size());
	tmp = tmp.substr(0, tmp.find("\r\n"));
	if (tmp != "chunked") { return true; }
	if (buf.find("0\r\n\r\n") == std::string::npos) { return false; }
	return true;
}

static bool	lengthCheck(const std::string& buf, const std::vector<std::string> tokens) {
	if (vectorSearch(tokens, "Transfer-Encoding: chunked")) { return true; }

	const std::string tmp = "Content-Length: ";

	if (!vectorSearch(tokens, tmp)) { return true; }

	size_t pos1 = buf.find(tmp) + (tmp.size());
	std::string num = buf.substr(pos1);
	num = num.substr(0, num.find("\r\n"));
	std::istringstream ss(num);
	size_t n;
	ss >> n;
	size_t pos2 = buf.find("\r\n\r\n");
	std::string a = buf.substr(pos2 + 4);
	if (a.size() < n) { return false; }
	return true;
}

static bool	boudaryCheck(const std::string& buf, const std::vector<std::string> tokens) {
	std::string tmp = "Content-Type: multipart/form-data; boundary=";
	//if (buf.find(tmp) == std::string::npos) { return true; }
	if (!vectorSearch(tokens, tmp)) { return true; }

	std::string boudary = buf.substr(buf.find(tmp) + tmp.size());
	boudary = boudary.substr(0, boudary.find("\r\n"));
	size_t pos1 = buf.find("--" + boudary);
	if (pos1 == std::string::npos) { return false; }
	size_t pos2 = buf.rfind("--" + boudary + "--");
	if (pos2 == std::string::npos) { return false; }
	return true;
}

static bool	isNum(const std::string &s) {
	for (size_t i = 0; i < s.size(); i += 1) {
		if (std::isdigit(s[i]) == 0) {
			return false;
		}
	}
	return true;
}

static bool	isUppercase(const std::string &s) {
	const size_t size = s.size();
	for (size_t i = 0; i < size; i += 1) {
		if (std::isupper(s[i]) == 0)
			return false;
	}
	return true;
}

static int	isRequestLine(const std::string &s) {
	std::string	tmp;
	size_t pos = s.find("\r\n");
	if (pos != std::string::npos) {
		tmp = s.substr(0, pos);
	} else {
		tmp = s;
	}

	std::vector<std::string> strs = ftSplit(tmp, " ");
	// GET / HTTP/1.1 https://wa3.i-3-i.info/word1843.html
	// 上記のようなリクエストラインを" "でスプリットした結果、数が3つでないため400
	if (strs.size() != 3) { return 400; }

	//METHODが空文字もしくは、大文字でない場合は400
	if (strs[0] == "" || !isUppercase(strs[0])) {
		return 400;
	}

	// 下記の3つ以外で文法的に正しいMETHODが来た場合は501
	if (!(strs[0] == "GET" || strs[0] == "POST" || strs[0] == "DELETE")) {
		return 501;
	}

	// pathが"/"で始まらない場合は400
	if (strs[1].find("/") != 0) { return 400; }

	// httpversionがHTTP/で始まっていない場合は400
	if (strs[2].find("HTTP/") != 0) {
		return 400;
	}
	// "HTTP/1.1"でない場合は400もしくは505のどちらか
	if (strs[2] != "HTTP/1.1") {
		std::string tmp = strs[2].substr(5);
		size_t pos = tmp.find(".");
		if (pos != std::string::npos) {
			tmp = tmp.substr(0, pos);
		}
		//"."の前の文字が数字でない場合は400
		if (!isNum(tmp)) {
			return 400;
		}
		int n = 400;
		//"."の前の文字の数字が1でない場合は505
		if (ftStoi(tmp) != 1) {
			n = 505;
		}
		return n;
	}
	return 0;
}


int	Server::_Recv(int fd, Connection &c) {
	if (c.getConnectionStat() != RECV) { return 0; }
	c.setConnectingTime(std::time(NULL));
	if (c.getRequestInfo().getBuf().empty()) {
		c.setReadTime(std::time(NULL));
		if (c.getRequestAttemps().second > REQUESTLIMITS) {
			c.setConnectionStat(ERROR);
			c.setErrStatus(429);
			return 1;
		} else {
			c.incrementAttemps();
		}
	}
	// 408　一定時間以上recvに時間がかかる場合は408
	if (TIMEOUT408 < difftime(std::time(NULL), c.getReadTime())) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(408);
		return 1;
	}

	char	buf[BUF_SIZE + 1];
	ssize_t	ret;
	std::memset(buf, 0, sizeof(buf));

	ret = recv(fd, buf, BUF_SIZE, 0);
	if (ret < 0) {
		errMessage("recv");
		c.setConnectionStat(CLOSE);
		return 1;
	} else if (ret == 0) {
		std::cout << "recv 接続が切れた -> [" << fd << "]" << std::endl;
		c.setConnectionStat(CLOSE);
		return 1;
	} 
	std::string	b(buf, ret);
	std::cout << "recv [" << fd << "] " << ret << "bytes" << std::endl;
// Bufferの中身がなく、かつ、今回のイベントで空行が送られてきた場合はBufferに追加せずに終了。次も再度Recvする
	if (c.getRequestInfo().getBuf().empty() && b == "\r\n") {
		return 0;
	}

	c.getRequestInfo().addBuf(b);

	int	err = isRequestLine(c.getRequestInfo().getBuf());
	if (err != 0) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(err);
		return 1;
	}

// ここではbufferを見て、空行がないため、リクエストメッセージが完了していないと判断し、次も再度RECVする。
	if (c.getRequestInfo().getBuf().find("\r\n\r\n") == std::string::npos) {
		return 0;
	}

	const std::string	&request(c.getRequestInfo().getBuf());
	// GET と　DELETEの場合はBodyは不要のため、終わらす
	if (request.find("GET") == 0 || request.find("DELETE") == 0) {
		c.setConnectionStat(PARSE);
		return 1;
	}
	std::string tmp(request.substr(0, request.find("\r\n\r\n")));
	std::vector<std::string> tokens = ftSplit(tmp, "\r\n");
	// Transfer-Encoding: chunked と Content-Lengthの二つが
	// ヘッダーから欠如しているため、bodyの終わりを判断できないため、411を返す
	if (!vectorSearch(tokens, "Transfer-Encoding: chunked")
	&& !vectorSearch(tokens, "Content-Length: ")) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(411);
		return 1;
	}

	if (boudaryCheck(request, tokens) == false) { return 0; }
	if (lengthCheck(request, tokens) == false) { return 0; }
	if (chunkCheck(request, tokens) == false) { return 0; }

	c.setConnectionStat(PARSE);
	return 1;
}