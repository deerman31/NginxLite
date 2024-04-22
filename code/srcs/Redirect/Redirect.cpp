#include "Server.hpp"

int	Server::_Redirect(Connection &c) {
	if (c.getConnectionStat() != REDIRECT) {
		return 0;
	}
	const ConfigServer	&servConfig = c.setConfig();
	const Line			&line = c.getRequestInfo().getLine();
	const Location		&location = c.nowLocation(servConfig);
	const std::string r("\r\n");
	std::string	response;
	response += line.httpv + " " + resStatusCode(301) + r;
	response += "Location: " + location.redirect + r;
	response += "Server: webserv";
	response += "Date: " + _getRFC1123Date() + r;
	response += "Content-Length: 0" + r;
	response += "Connection: close" + r + r;
	c.setSendMessage(response);
	c.setEvent(POLLOUT);
	c.setConnectionStat(SEND);
	return 0;
}