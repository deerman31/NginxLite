#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <sstream>

#define BUF_SIZE 1025

ssize_t	ft_recv(int fd) {
	char	buf[BUF_SIZE];
	ssize_t	ret;

	ret = recv(fd, buf, (BUF_SIZE - 1), 0);
	if (ret < 0) {
		perror("recv");
		return -1;
	} else if (ret == 0) {
		std::cout << "recv 接続が切れた: " << fd << std::endl;
		return ret;
	}
	std::string	s(buf, ret);
	std::cout << "-----------リクエストメッセージ-----------" << std::endl;
	std::cout << s << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	return ret;
}

ssize_t	ft_send(int fd, const std::string &messeage) {
	ssize_t				ret;
	std::string			response;
	std::ostringstream	oss;

	oss << messeage.length() + 2;
	response += "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "Content-Length: ";
	response += oss.str() + "\r\n";
	response += "\r\n";
	response += messeage;
	response += "\r\n";

	ret = send(fd, response.c_str(), response.length(), 0);
	if (ret < 0) {
		perror("send");
		return -1;
	} else if (ret == 0) {
		std::cout << "send 接続が切れた: " << fd << std::endl;
		return ret;
	}
	return ret;
}

int	ft_accept(int listen_fd) {
	int					fd;
	struct sockaddr_in	address;
	int addrlen = sizeof(address);

	fd = accept(listen_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
	if (fd < 0) {
		perror("accept");
		return -1;
	}
	// fdをノンブロッキングに設定
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl()");
		return -1;
	}
	return fd;
}

int	ft_listen(int port_num) {
	int					fd;
	struct sockaddr_in	address;
	int					opt;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket()");
		return -1;
	}
	opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt()");
		close(fd);
		return -1;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port_num);
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind()");
		close(fd);
		return -1;
	}
	if (listen(fd, 3) < 0) {
		perror("listen()");
		close(fd);
		return -1;
	}
	// fdをノンブロッキングに設定
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl()");
		close(fd);
		return -1;
	}
	return fd;
}

void	server_close(int n, std::vector<pollfd> &listens, std::vector<pollfd> &connections) {
	for (std::vector<pollfd>::iterator it = listens.begin(); it != listens.end(); ++it) {
		if (it->fd != -1) {
			close(it->fd);
		}
	}
	for (std::vector<pollfd>::iterator it = connections.begin(); it != connections.end(); ++it) {
		if (it->fd != -1) {
			close(it->fd);
		}
	}
	if (n == EXIT_FAILURE) {
		std::exit(EXIT_FAILURE);
	}
}

int	ft_poll(std::vector<pollfd>	&listen_fds, std::vector<pollfd> &connection_fds) {
	int					ret;
	std::vector<pollfd>	tmp;

	// poll()は一つのリストでなければ使えないため、tmpに二つのvectorを追加する
	// listen_fdsをtmpに追加する
	for (std::vector<pollfd>::const_iterator it = listen_fds.begin(); it != listen_fds.end(); ++it) {
		tmp.push_back(*it);
	}
	// connection_fdsをtmpに追加する
	for (std::vector<pollfd>::const_iterator it = connection_fds.begin(); it != connection_fds.end(); ++it) {
		tmp.push_back(*it);
	}
	// 第３引数はブロックする時間
	// 
	ret = poll(tmp.data(), tmp.size(), 100);
	if (ret < 0) {
		perror("poll");
		return -1;
	} else if (ret == 0) {
		// 戻り値が0ということはイベントが発生していない
		std::cout << "TIMEOUT!" << std::endl;
		return 0;
	}
	// pollを通すとreventsフィールドにビットが立つので書くvectorのreventsを更新する
	std::vector<pollfd>::const_iterator t = tmp.begin();
	for (std::vector<pollfd>::iterator it = listen_fds.begin(); it != listen_fds.end(); ++it) {
		it->revents = t->revents;
		++t;
	}
	for (std::vector<pollfd>::iterator it = connection_fds.begin(); it != connection_fds.end(); ++it) {
		it->revents = t->revents;
		++t;
	}
	return ret;
}

int	main() {
	pollfd	server_fd, new_socket;
	// listen_fdsは新規接続を待ち受けるpollfdを管理するvector
	// connection_fdsはrecv(), send()を行うpollfdを管理するvector
	std::vector<listen>	listen_fds;
	std::vector<listen>	listen_fds;
	std::vector<pollfd>	listen_fds, connection_fds;

	// 8080で接続を待ち受けるsocket_fdを作成
	server_fd.fd = ft_listen(8080);
	if (server_fd.fd < 0) {
		std::exit(EXIT_FAILURE);
	}
	// 新規接続がきたかどうかは読み込みイベントのため、eventsフィールドにPOLLINをセット
	server_fd.events = POLLIN;
	// listen_fdsに追加
	listen_fds.push_back(server_fd);

	// 4242で接続を待ち受けるsocket_fdを作成
	server_fd.fd = ft_listen(4242);
	if (server_fd.fd < 0) {
		server_close(EXIT_FAILURE, listen_fds, connection_fds);
	}
	server_fd.events = POLLIN;
	listen_fds.push_back(server_fd);

	while (true) {
		int ret = ft_poll(listen_fds, connection_fds);
		if (ret < 0) {
			server_close(EXIT_FAILURE, listen_fds, connection_fds);
		} else if (ret == 0) {
		// retが0ということは、イベントが発生していないということなのでループの先頭に
			continue;
		}

		// listen_fdsをループで回し、reventsにビットが立っている要素を探す
		for (std::vector<pollfd>::iterator it = listen_fds.begin(); it != listen_fds.end(); ++it) {
			// (it->revents & POLLIN)のような書き方をすることでreventsにPOLLINのビットが立っているかを判別する
			if (it->revents & POLLIN) {
			// フラグが立っていたら、ft_acceptを用い、接続済みのsocket_fdを作成
				new_socket.fd = ft_accept(it->fd);
				if (new_socket.fd < 0) {
					server_close(EXIT_FAILURE, listen_fds, connection_fds);
				}
				// 接続したfdが最初にやるべきことはクライアントからの読み込みであるため、POLLINをセットする
				new_socket.events = POLLIN;
				new_socket.revents = 0;
				// connection_fdsに追加
				connection_fds.push_back(new_socket);
			}
		}
		// connection_fdsをループで回し、reventsにフラグが立っている要素を探す
		for (std::vector<pollfd>::iterator it = connection_fds.begin(); it != connection_fds.end();) {
			// POLLINのビットが立っていたら、ft_recvでリクエストメッセージを受信する
			if (it->revents & POLLIN) {
				ssize_t ret = ft_recv(it->fd);
				if (ret < 0) {
					server_close(EXIT_FAILURE, listen_fds, connection_fds);
				} else if (ret == 0) {
					// 0が帰ってきたら、クライアント側で接続を切ったということであるため、
					// vectorから削除する必要がある
					close(it->fd);
					it = connection_fds.erase(it);
					continue;
				} else {
					// 読み込みイベントが終わったら、次にするのは書き込みイベントであるため、eventsフィールドにPOLLOUTをセットする
					it->events = POLLOUT;
				}
			// POLLOUTのビットが立っていたら、ft_sendでレスポンスメッセージを送信する
			} else if (it->revents & POLLOUT) {
				ssize_t ret = ft_send(it->fd, "Hello,world!");
				if (ret < 0) {
					server_close(EXIT_FAILURE, listen_fds, connection_fds);
				} else if (ret == 0) {
					// 0が帰ってきたら、クライアント側で接続を切ったということであるため、
					// vectorから削除する必要がある
					close(it->fd);
					it = connection_fds.erase(it);
					continue;
				} else {
					// 書き込みイベントが終わったので、読み込みイベントを待つためにPOLLINをセットする
					it->events = POLLIN;
				}
			}
			++it;
		}
	}
	server_close(EXIT_SUCCESS, listen_fds, connection_fds);
	return 0;
}