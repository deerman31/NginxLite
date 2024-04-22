#include "Server.hpp"

Line	requestLineHandle(const std::string& line) {
	Line	reqline;
	std::vector<std::string> tokens = ftSplit(line, " ");

	reqline.method = tokens[0];	
	reqline.path = tokens[1];
	reqline.httpv = tokens[2];
	return reqline;
}

Headers	headHandle(std::vector<std::string> strs) {
	Headers	head;
	std::string key, value;

	head.connection = true; // Connectionがヘッダーにない場合は defaultでkeep-alive

	for (size_t i = 1; i < strs.size(); i += 1) {
		if (strs[i].size() == 0) {
			break;
		}
		size_t pos = strs[i].find(":");
		key = strs[i].substr(0, pos);
		value = strs[i].substr(pos + 2);
		if (key == "Host") {
			head.host = value;
		} else if (key == "Content-Length") {
			head.contentLength = value;
		} else if (key == "Content-Type") {
			head.contentType = value;
		} else if (key == "User-Agent") {
			head.userAgent = value;
		} else if (key == "Accept") {
			head.accept = value;
		} else if (key == "Transfer-Encoding") {
			head.transEncoding = value;
		} else if (key == "Location") {
			head.location = value;
		} else if (key == "Connection") {
			if (value == "keep-alive") { head.connection = true; }
			else if (value == "close") { head.connection = false; }
		}
		head.otherHeaders[key] = value;
	}
	return head;
}

static bool	isValue(const std::string& value) {
	for (size_t i = 0; i < value.length(); i += 1) {
		if (value[i] < 0 || value[i] > 127) { return false; } // ascii文字以外はダメ
	}
	return true;
}

static bool	isFieldName(const std::string& line) {
	for (size_t i = 0 ; i < line.length(); i += 1) {
		if (!(std::isalnum(line[i]) == 1 || line[i] == '-')) { return false; } //英数字と-以外はダメ
	}
	return true;
}

static bool	checkHost(const std::vector<std::string>& lines) {
	size_t num = 0;
	for (size_t i = 1; i < lines.size(); i += 1) {
		if (lines[i].find("Host: ") == 0) {
			num += 1;
		}
	}
	if (num == 1) { return true; }
	return false;
}

static bool	checkDuplication(const std::vector<std::string>& lines) {
	size_t		num;
	std::string	tmp;

	for (size_t i = 1; i < lines.size(); i += 1) {
		if (lines[i] == "") { break; }
		num = 0;
		for (size_t j = 1; j < lines.size(); j += 1) {
			tmp = lines[j].substr(0, lines[j].find(": ") + 1);
			if (tmp == "") { break; }
			if (tmp != "" && lines[i].find(tmp) == 0) { num += 1; }
		}
		if (num != 1) { return false; }
	}
	return true;
}

static const std::pair<std::string, std::string>	pairSplit(const std::string& s, const std::string& del) {
	std::pair<std::string, std::string>	pair;
	size_t pos = s.find(del);
	pair.first = s.substr(0, pos);
	pair.second = s.substr(pos + del.size());
	return pair;
}

static bool	isRequest(const std::vector<std::string>& tokens) {
	// tokensのサイズが1個ということはリクエストラインのみなので400
	if (tokens.size() < 2) { return false; }
	//Hostヘッダーがない場合はリクエストとしては足りないため400
	if (!checkHost(tokens)) { return false; }
	// ヘッダーに重複があるため400
	if (!checkDuplication(tokens)) { return false; }
	std::pair<std::string, std::string> pairs;
	for (size_t i = 1; i < tokens.size(); i += 1) {
		if (tokens[i] == "") { break; }
		// コロン":"がないため、400
		if (tokens[i].find(": ") == std::string::npos) { return false; } // コロンスペースの欠如チェック
		// 以下はsplitの際に"\r\n"はなくなっているはずだから、その中で以下の特殊文字がある場合は400
		if (tokens[i].find("\r\n") != std::string::npos) { return false; } // \r\nがあってはいけない
		if (tokens[i].find("\n") != std::string::npos) { return false;} // \nがあるとダメ linesの作る際に\r\nは無くなっているはずだから。
		if (tokens[i].find("\r") != std::string::npos) { return false;} // \rがあるとダメ
		pairs = pairSplit(tokens[i], ": ");
		//英数字と-以外がヘッダーの左側にある場合400
		if (!isFieldName(pairs.first)) { return false; }
		//ascii文字以外がヘッダーの右側にある場合400
		if (!isValue(pairs.second)) { return false; }
	}
	return true;
}

Body	setBody(const std::vector<std::string> &lines) {
	std::vector<std::string>::const_iterator it;
	Body		body;
	std::string length;
	std::string type;
	std::string text;

	for (it = lines.begin(); it != lines.end(); ++it) {
		if (it->find("Content-Length: ") == 0) {
			length = it->substr(16);
		} else if (it->find("Content-Type: ") == 0) {
			type = it->substr(14);
		} else if (*it == "") {
			++it;
			break;
		}
	}
	for (; it != lines.end(); ++it) {
		text += *it + "\r\n";
	}
	if (text.length() > 2) {
		text.resize(text.length() - 2);
	}
	body.contentLength = length;
	body.contentType = type;
	body.body = text;
	return body;
}

int	Server::_Parse(Connection &c) {
	if (c.getConnectionStat() != PARSE) { return 0; }
	std::vector<std::string> tokens = ftSplit(c.getRequestInfo().getBuf(), "\r\n");
	c.getRequestInfo().clearBuf();

	// Requestに不備があるため400
	if (!isRequest(tokens)) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(400);
		return 0;
	}

	Line	line(requestLineHandle(tokens[0]));
	Headers	headers(headHandle(tokens));
	Body	body;

	if (line.method == "POST") {
		body = setBody(tokens);
	}
	c.getRequestInfo().setLine(line);
	c.getRequestInfo().setHeader(headers);
	c.getRequestInfo().setBody(body);
	c.setKeep(headers.connection);

	// urlが一定よりも長い場合には414
	std::string url("http://" + headers.host + line.path);
	if (url.length() > MAX_URL) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(414);
		return 0;
	}
	size_t size = 0;
	std::map<std::string, std::string>::iterator it = headers.otherHeaders.begin();
	for (; it != headers.otherHeaders.end(); ++it) {
		size += it->first.length() + it->second.length() + 2;
		// ヘッダーの総文字数が一定以上になると431
		if (size > MAX_HEADER_SIZE) {
			c.setConnectionStat(ERROR);
			c.setErrStatus(431);
			return 0;
		}
	}
	if (line.method == "GET") {
		c.setConnectionStat(GET);
	} else if (line.method == "POST") {
		c.setConnectionStat(POST);
	} else if (line.method == "DELETE") {
		c.setConnectionStat(DELETE);
	}
	return 0;
}
