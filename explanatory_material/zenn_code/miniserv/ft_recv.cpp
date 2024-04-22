#include "echoserv.hpp"

ssize_t	ft_recv(Connection &connection) {
	char	buf[BUF_SIZE];
	ssize_t	ret;

	// recv(): クライアントからのリクエストメッセージを受信し、bufに格納する
	ret = recv(connection.pfd.fd, buf, (BUF_SIZE - 1), 0);
	if (ret < 0) {
		perror("recv");
		return -1;
	} else if (ret == 0) {
		// 0が返ってきたら、クライアント側が一方的に接続を切ったということ
		std::cout << "recv 接続が切れた: " << connection.pfd.fd << std::endl;
		return ret;
	}
	// 構造体のbufferにリクエストメッセージを格納
	connection.buffer = std::string(buf, ret);
	std::cout << "-----------リクエストメッセージ-----------" << std::endl;
	std::cout << connection.buffer << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	return ret;
}