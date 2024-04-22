// <sys/socket.h>: ソケットプログラミングのための基本的な関数とデータ構造を提供します。
#include <sys/socket.h>
// <netinet/in.h>: 特にsockaddr_in構造体の定義が含まれ、インターネット操作に必要です。
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sstream>

// recv()で格納するbufferのサイズ
#define BUF_SIZE 1024
// 接続に使用するポート番号
#define PORT_NUM 8080

int	main() {
	int	server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buf[BUF_SIZE];

	// AF_INET: IPv4 インターネットプロトコルを使用することを指定します。
	// SOCK_STREAM: 信頼性の高い、接続指向のバイトストリームを提供します。
	// socket(): 接続を待ち受けるsocketのFDを作成します。
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket()");
		std::exit(EXIT_FAILURE);
	}

	// ソケットオプションの設定（setsockopt()）：
	// SO_REUSEADDR: 同じポートに対して早く再バインドできるようにするオプションを設定します。
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// アドレスファミリをIPv4に設定します。
	address.sin_family = AF_INET;
	// サーバが任意のIPアドレスから接続を受け入れられるようにします。
	address.sin_addr.s_addr = INADDR_ANY;
	// ポート番号をネットワークバイトオーダーに変換して設定します。
	address.sin_port = htons(PORT_NUM);

	// 指定されたポートとアドレスをソケットに関連付けます。
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// サーバが接続要求を待ち受けるように設定します。3はバックログ（保留中の接続のキューの最大長）です。
	if (listen(server_fd, 3) < 0) {
		perror("listen()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// クライアントからの接続を受け入れ、新たなソケットnew_socketを作成します。
	new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	if (new_socket < 0) {
		perror("accept()");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// クライアントから送られてくるリクエストメッセージを受け取り、bufに格納します。
	// BUF_SIZE - 1 は末尾のナル文字分を確保するため
	// recv()はクライアントから送られてくるメッセージを受信できる
	ssize_t ret = recv(new_socket, buf, (BUF_SIZE - 1), 0);
	if (ret < 0) {
		perror("recv()");
		close(server_fd);
		close(new_socket);
		exit(EXIT_FAILURE);
	}

	// charの配列からstringに変換
	std::string	str(buf, ret);

	std::cout << "-----------リクエストメッセージ-----------" << std::endl;
	std::cout << str << std::endl;
	std::cout << "---------------------------------------" << std::endl;

	// 下記のような形式にしないとクライアントはメッセージを認識できない
	std::ostringstream	oss;
	oss << "content-length: " << (str.length() + 2) << "\r\n";
	std::string response;
	response += "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: text/plain\r\n";
	response += oss.str();
	response += "\r\n";
	response += str + "\r\n";

	// send()はrecv()の反対でクライアントにメッセージを送信できる
	ret = send(new_socket, response.c_str(), response.length(), 0);
	if (ret < 0) {
		perror("send()");
		close(server_fd);
		close(new_socket);
		exit(EXIT_FAILURE);
	}
	std::cout << "\"Hello,world!\" message sent to client." << std::endl;

	// 作成したfdをcloseする
	close(server_fd);
	close(new_socket);
	return 0;
}
