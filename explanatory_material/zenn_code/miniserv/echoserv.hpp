#ifndef ECHOSERV_HPP
# define ECHOSERV_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <poll.h>
# include <cstring>
# include <iostream>
# include <fcntl.h>
# include <vector>
# include <sstream>

#define BUF_SIZE 1025

struct Listen {
	// port番号
	int		port_num;
	// fdやeventの種類と発生を管理するpollfdの構造体
	pollfd	pfd;
};

struct Connection {
	// port番号
	int			port_num;
	// fdやeventの種類と発生を管理するpollfdの構造体
	pollfd		pfd;
	// 受信したリクエストメッセージをクライアントにそのまま送り返すために保存するbuffer
	std::string	buffer;
};

bool	set_port(int, const char**, std::vector<int>&);
int		ft_accept(int);
int		ft_listen(int);
int		ft_poll(std::vector<Listen>&, std::vector<Connection>&);
ssize_t	ft_recv(Connection&);
ssize_t	ft_send(Connection&);
void	server_close(int, std::vector<Listen>&, std::vector<Connection>&);

#endif