#include "Server.hpp"

static const std::string	resHtml() {
	std::string r;
	r = "<!DOCTYPE html>";
	r += "<html lang=\"en\">";
	r += "<head>";
	r += "<meta charset=\"UTF-8\">";
	r += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
	r += "<title>Upload Successful</title>";
	r += "<style>";
	r += "body {";
	r += "font-family: Arial, sans-serif;";
	r += "text-align: center;";
	r += "margin-top: 50px;";
	r += "}";
	r += ".message {";
	r += "color: #4CAF50;";
	r += "margin: 20px;";
	r += "}";
	r += ".upload-button {";
	r += "background-color: #008CBA;";
	r += "color: white;";
	r += "padding: 10px 20px;";
	r += "margin: 20px;";
	r += "border: none;";
	r += "border-radius: 5px;";
	r += "cursor: pointer;";
	r += "}";
	r += ".upload-button:hover {";
	r += "background-color: #005f73;";
	r += "}";
	r += "</style>";
	r += "</head>";
	r += "<body>";
	r += "<h1>Upload Successful!</h1>";
	r += "<p class=\"message\">Your file has been successfully uploaded.</p>";
	r += "</body>";
	r += "</html>";
	return r;
}

static std::string	fileNameCreate(const std::string &path) {
	std::string	res;
	std::string	n;
	std::string tmp;
	if (path.back() == '/') {
		tmp = path + "upload";
	} else {
		tmp = path + "/upload";
	}

	for (size_t i = 1; i <= SIZE_MAX; i += 1) {
		n = ftItoa(i);
		if (ftStat((tmp + n).c_str()) != 1) {
			res = tmp + n;
			break;
		}
	}
	return res;
}

int	Server::_Post(Connection &c) {
	if (c.getConnectionStat() != POST) { return 0; }

	const ConfigServer &servConfig = c.setConfig();
	const Location &l = c.nowLocation(servConfig);
	const Line &line = c.getRequestInfo().getLine();
	const Headers header = c.getRequestInfo().getHeader();
	const Body &body = c.getRequestInfo().getBody();

	std::string path(l.uploadPath);

	int n = ftStat(path.c_str());
	if (n != 2 && path.rfind("/") != 0) {
		path = path.substr(0, path.rfind("/"));
		n = ftStat(path.c_str());
	}
	if (n != 2) {
		// directoryは2のため２以外だと404
		c.setConnectionStat(ERROR);
		// nが1ということはファイルであり、ファイルにpostはできないため、その名前のdirectoryがないという意味で404
		if (n == 1) {
			n = 404;
		}
		c.setErrStatus(n);
		return 0;
	}

	std::string	fileName;
	if (body.filename.empty()) {
		fileName = fileNameCreate(path);
	} else {
		fileName = path.back() == '/' ? path + body.filename : path + "/" + body.filename;
	}

	// 該当のメディアタイプだと415
	if (_isMediaType(fileName)) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(415);
		return 0;
	}

	Response	resInfo;
	std::string	response;
	if (ftStat(fileName.c_str()) == 1) {
		//statで調べて、1ということは該当のファイルがあるため、データの更新のみであり、bodyはないた204
		resInfo.status = resStatusCode(204);
	} else {
		// 新規作成ができたため201
		resInfo.status = resStatusCode(201);
		response = resHtml();
	}
	std::ofstream output(fileName.c_str());
	if (!output) {
		std::cerr << fileName << std::endl;
		errMessage("open");
		c.setConnectionStat(ERROR);
		c.setErrStatus(415);
		return 0;
	}

	output << body.body;

	output.close();
	resInfo.contentType = "text/html; charset=UTF-8";
	resInfo.httpV = line.httpv;
	resInfo.serverName = "webserv";
	resInfo.date = _getRFC1123Date();
	resInfo.location = path;
	resInfo.contentLength = ftItoa(response.size());
	if (header.connection) {
		resInfo.connection = "keep-alive";
	} else {
		resInfo.connection = "close";
	}
	if (resInfo.status == resStatusCode(201)) {
		resInfo.resource = response;
	}

	c.setResponse(resInfo);
	c.setConnectionStat(SEND);
	c.setEvent(POLLOUT);
	return 0;
}