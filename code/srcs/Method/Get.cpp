#include "Server.hpp"

int	Server::_Get(Connection &c) {
	if (c.getConnectionStat() != GET) { return 0; }

	const ConfigServer	&servConfig = c.setConfig();
	const Line			&line = c.getRequestInfo().getLine();

	std::string	path(c.getPath(servConfig));

	int	ret = ftStat(path.c_str());
	// statの結果、file dir がない場合は404 あるが、権限がない場合は403
	if (ret > 2) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(ret);
		return 0;
	}
	// 該当のメディアタイプだと415
	if (_isMediaType(path)) {
		c.setConnectionStat(ERROR);
		c.setErrStatus(415);
		return 0;
	}
	if (ret == 2 && path.back() != '/') {
		path += "/";
	}
	std::string	s;
	const Location &location = c.nowLocation(servConfig);
	std::string locapath;
	Response	resInfo;
	if (location.path.back() != '/') { locapath = location.path + "/";
	} else { locapath = location.path; }
	if (locapath == line.path) {
		if (location.indexFiles.size() != 0) {
			std::vector<std::string>::const_iterator it;
			for (it = location.indexFiles.begin(); it != location.indexFiles.end(); ++it) {
				ret = ftStat((path + *it).c_str());
				if (ret == 1) {
					if (!_readFileContents(s, path + *it)) {
						c.setConnectionStat(ERROR);
						c.setErrStatus(500);
						return 0;
					}
					//resInfo.resource = _readFileContents(path + *it);
					resInfo.resource = s;
					resInfo.contentType = setContentsType(path + *it);
					resInfo.lastModified = _getLastModified(path + *it);
					break;
				}
			}
			if (it == location.indexFiles.end()) {
				// locationのpathとリクエストpathが同一であり、indexfileが設定されていない、かつ、autoindexがonじゃない場合は404
				if (location.autoIndex == false) {
					c.setConnectionStat(ERROR);
					c.setErrStatus(404);
					return 0;
				} else {
					resInfo.resource = _listDirContents(path, location);
				}
			} else {
				// indexFileがあるが、実際にそのindexfileがなかったり、権限がない場合は404
				if (ret != 1) {
					c.setConnectionStat(ERROR);
					c.setErrStatus(ret);
					return 0;
				}
			}
		} else {
			if (location.autoIndex) {
				resInfo.resource = _listDirContents(path, location);
			} else {
				// autoindexがないため、404
				c.setConnectionStat(ERROR);
				c.setErrStatus(404);
				return 0;
			}
		}
	} else {
		if (ret == 2) {
			if (location.autoIndex) {
				resInfo.resource = _listDirContents(path, location);
			} else {
				// retが２ということはdirectoryであり、autoindexがonでないため、404
				c.setConnectionStat(ERROR);
				c.setErrStatus(404);
				return 0;
			}
		} else if (ret == 1) {
			if (!_readFileContents(s, path)) {
				c.setConnectionStat(ERROR);
				c.setErrStatus(500);
				return 0;
			}
			resInfo.resource = s;
			//resInfo.resource = _readFileContents(path);
		}
	}
	resInfo.httpV = line.httpv;
	resInfo.status = "200 OK";
	resInfo.serverName = "webserv";
	resInfo.date = _getRFC1123Date();
	if (resInfo.contentType.empty()) {
		resInfo.contentType = setContentsType(path);
	}
	if (resInfo.lastModified.empty()) {
		resInfo.lastModified = _getLastModified(path);
	}
	resInfo.contentLength = ftItoa(resInfo.resource.length());
	if (c.getRequestInfo().getHeader().connection) {
		resInfo.connection = "keep-alive";
	} else {
		resInfo.connection = "close";
	}
	c.setResponse(resInfo);
	c.setEvent(POLLOUT);
	c.setConnectionStat(SEND);
	return 0;
}