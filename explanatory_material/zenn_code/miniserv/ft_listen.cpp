#include "echoserv.hpp"

int	ft_listen(int port_num) {
	int					fd;
	struct sockaddr_in	address;
	int					opt;

	// AF_INET: IPv4 インターネットプロトコルを使用することを指定します
	// SOCK_STREAM: 信頼性の高い、接続指向のバイトストリームを提供します
	// socket(): 接続を待ち受けるsocketのFDを作成します
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket()");
		return -1;
	}

	opt = 1;
	// setsockopt(): ソケットオプションの設定
    // SO_REUSEADDR: 同じポートに対して早く再バインドできるようにするオプションを設定します
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt()");
		close(fd);
		return -1;
	}

	// AF_INET: アドレスファミリをIPv4に設定します
	address.sin_family = AF_INET;
	// INADDR_ANY: ポート番号をネットワークバイトオーダーに変換して設定します
	address.sin_addr.s_addr = INADDR_ANY;
	// htons(): ポート番号をネットワークバイトオーダーに変換して設定します
	address.sin_port = htons(port_num);

	// bind(): 指定されたポートとアドレスをソケットに関連付けます
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind()");
		close(fd);
		return -1;
	}

	// listen(): サーバが接続要求を待ち受けるように設定します。3はバックログ（保留中の接続のキューの最大長）です
	if (listen(fd, 3) < 0) {
		perror("listen()");
		close(fd);
		return -1;
	}
	// fcntl(): fdをノンブロッキングに設定
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl()");
		close(fd);
		return -1;
	}
	return fd;
}
