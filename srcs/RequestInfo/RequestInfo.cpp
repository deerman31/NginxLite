#include "RequestInfo.hpp"

RequestInfo::RequestInfo() {}
RequestInfo::~RequestInfo() {}
RequestInfo::RequestInfo(const RequestInfo &src):
_buf(src._buf), _line(src._line), _headers(src._headers), _body(src._body) {}
RequestInfo	&RequestInfo::operator=(const RequestInfo &src) {
	if (this == &src) { return *this; }
	_buf = src._buf;
	_line = src._line;
	_headers = src._headers;
	_body = src._body;
	return *this;
}

const std::string	&RequestInfo::getBuf() const { return _buf; }
void	RequestInfo::addBuf(const std::string &b) { _buf += b; }
void	RequestInfo::clearBuf() { _buf.clear(); }

const Line	&RequestInfo::getLine() const { return _line; }
void	RequestInfo::setLine(const Line &line) { _line = line; }
void	RequestInfo::clearLine() {
	_line.path.clear();
	_line.method.clear();
	_line.httpv.clear();
}

const Headers	&RequestInfo::getHeader() const { return _headers; }
void	RequestInfo::setHeader(const Headers &header) { _headers = header; }
void	RequestInfo::clearHeader() {
	_headers.otherHeaders.clear();
	_headers.host.clear();
	_headers.contentLength.clear();
	_headers.contentType.clear();
	_headers.userAgent.clear();
	_headers.accept.clear();
	_headers.connection = true;
	_headers.transEncoding.clear();
	_headers.location.clear();
}

const Body	&RequestInfo::getBody() const { return _body; }
void	RequestInfo::setBody(const Body &b) { _body = b; }
void	RequestInfo::clearBody() {
	_body.body.clear();
	_body.contentType.clear();
	_body.contentLength.clear();
	_body.filename.clear();
}