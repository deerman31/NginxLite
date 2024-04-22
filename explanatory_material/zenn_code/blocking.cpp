#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define BUF_SIZE 1024
#define PORT_NUM 8080

int	main() {
	int	server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buf[BUF_SIZE];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket()");
		std::exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT_NUM);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	size_t count = 1;
	while (true) {
		// ここでBlockingが起こっている
		std::cout << "accept()の前\n";
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (new_socket < 0) {
			perror("accept()");
			close(server_fd);
			exit(EXIT_FAILURE);
		}
		std::cout << "accept()の後\n";
		ssize_t ret = recv(new_socket, buf, (BUF_SIZE - 1), 0);
		if (ret < 0) {
			perror("recv()");
			close(server_fd);
			close(new_socket);
			exit(EXIT_FAILURE);
		}

		std::string	str(buf, ret);

		std::cout << "< " << count << " >" << std::endl;
		std::cout << "-----------リクエストメッセージ-----------" << std::endl;
		std::cout << str << std::endl;
		std::cout << "---------------------------------------" << std::endl;

		std::string response(
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 14\r\n"
			"\r\n"
			"Hello,world!\r\n");

		ret = send(new_socket, response.c_str(), response.length(), 0);
		if (ret < 0) {
			perror("send()");
			close(server_fd);
			close(new_socket);
			exit(EXIT_FAILURE);
		}
		std::cout << "\"Hello,world!\" message sent to client." << std::endl;
		/*
		ここでcloseしていたが、コメントアウトしよう。ここでcloseしなくても勝手に接続が切れるはずだ
		理由はこちらがレスポンスメッセージを適切な形で送ったので、
		クライアント側がレスポンスメッセージを全て受信したと判断し、クラアント側でcloseするため
		*/
		//close(new_socket);
		count += 1;
	}
	close(server_fd);
	return 0;
}
