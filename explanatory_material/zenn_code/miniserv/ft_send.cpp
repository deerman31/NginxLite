#include "echoserv.hpp"

ssize_t	ft_send(Connection &connection) {
	ssize_t				ret;
	std::string			response;
	std::ostringstream	oss;

	oss << connection.buffer.length() + 2;
	response += "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: ";
	response += oss.str() + "\r\n";
	response += "\r\n";
	response += connection.buffer;
	response += "\r\n";

	// send(): クライアントにレスポンスメッセージを送信する
	ret = send(connection.pfd.fd, response.c_str(), response.length(), 0);
	if (ret < 0) {
		perror("send");
		return -1;
	} else if (ret == 0) {
		// 0が返ってきたら、クライアント側が一方的に接続を切ったということ
		std::cout << "send 接続が切れた: " << connection.pfd.fd << std::endl;
		return ret;
	}
	return ret;
}