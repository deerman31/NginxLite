#ifndef REQUESTINFO_HPP
#define REQUESTINFO_HPP

#include <string>
#include <map>

struct Line {
	std::string	path;
	std::string	method;
	std::string	httpv;
};
struct Headers {
	std::map<std::string, std::string>	otherHeaders;
	std::string 						host;
	std::string 						contentLength;
	std::string 						contentType;
	std::string							userAgent;
	std::string							accept;
	bool								connection;
	std::string							transEncoding;
	std::string							location;
};

struct Body {
	std::string	body;
	std::string	contentType;
	std::string	contentLength;
	std::string	filename;
};

class RequestInfo {
	private:
	std::string	_buf;
	Line		_line;
	Headers		_headers;
	Body		_body;
	public:
	RequestInfo();
	~RequestInfo();
	RequestInfo(const RequestInfo &);
	RequestInfo	&operator=(const RequestInfo &);

	const std::string	&getBuf() const;
	void				addBuf(const std::string &);
	void				clearBuf();


	const Line	&getLine() const;
	void				setLine(const Line &);
	void				clearLine();

	const Headers	&getHeader() const;
	void				setHeader(const Headers &);
	void				clearHeader();

	const Body	&getBody() const;
	void				setBody(const Body &);
	void				clearBody();
};

#endif