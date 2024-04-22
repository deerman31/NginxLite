#include "echoserv.hpp"

int	main(int argc, const char **argv) {
	// listens: クライアントからの新規接続を待ち受けるSocketのvector
	std::vector<Listen>		listens;
	// connections: クライアントとの通信(IO操作)を行うSocketのvector
	std::vector<Connection>	connections;

	std::vector<int> ports;
	if (!set_port(argc, argv, ports)) {
		return 1;
	}
	//ここでコマンドライン引数から受け取ったPORT番号をもとにListenソケット作成し、Listensに追加
	for (std::vector<int>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
		Listen	listen;
		listen.port_num = *it;
		// ft_listens()でport_numをもとに新規接続を待ち受けるsocketのfdを作成
		listen.pfd.fd = ft_listen(listen.port_num);
		// 新規接続イベントは読み込みイベントなので、eventsフィールドにPOLLINをセットする
		listen.pfd.events = POLLIN;
		// vectorに追加
		listens.push_back(listen);
		if (listen.pfd.fd < 0) {
			server_close(EXIT_FAILURE, listens, connections);
		}
	}

	while (true) {
		// ft_poll()でイベントの発生を監視する
		// イベントが発生していれば、pollfdのreventsフィールドにビットが立つ
		int ret = ft_poll(listens, connections);
		if (ret < 0) {
			server_close(EXIT_FAILURE, listens, connections);
		} else if (ret == 0) {
			// 0ということはイベントが発生していないのでループの先頭に戻す
			continue;
		}
		for (std::vector<Listen>::iterator l = listens.begin(); l != listens.end(); ++l) {
			// (l->pfd.revents & POLLIN)のように書くとreventsフィールドにビットが立っているか判別できる
			if (l->pfd.revents & POLLIN) {
				Connection c;
				c.port_num = l->port_num;
				// ft_accept()で通信用のsocketfdを作成する
				c.pfd.fd = ft_accept(l->pfd.fd);
				if (c.pfd.fd < 0) {
					server_close(EXIT_FAILURE, listens, connections);
				}
				// connectionのsocketが最初にやるのは読み込みイベントのため、eventsフィールドにPOLLINをセットする
				c.pfd.events = POLLIN;
				c.pfd.revents = 0;
				// vectorに追加
				connections.push_back(c);
			}
		}
		for (std::vector<Connection>::iterator c = connections.begin(); c != connections.end();) {
			if (c->pfd.revents & POLLIN) {
				// ft_recv()でクライアントからのリクエストメッセージを受信し、コマンドラインに出力し、構造体のbufferに保存する
				ssize_t ret = ft_recv(*c);
				if (ret < 0) {
					server_close(EXIT_FAILURE, listens, connections);
				} else if (ret == 0) {
					// 0が返ってきたら、クライアント側が一方的に接続を切ったということなので
					// fdをcloseし、vectorから構造体を削除する
					close(c->pfd.fd);
					c = connections.erase(c);
					continue;
				}
				// 読み込みイベントが終わったら、次は書き込みイベントなのでeventsフィールドにPOLLOUTにセットする
				c->pfd.events = POLLOUT;
			} else if (c->pfd.revents & POLLOUT) {
				// ft_send()でクライアントにft_recv()で構造体のbufferに保存したリクエストメッセージをそのままレスポンスメッセージとして、送信する
				ssize_t ret = ft_send(*c);
				if (ret < 0) {
					server_close(EXIT_FAILURE, listens, connections);
				} else if (ret == 0) {
					// 0が返ってきたら、クライアント側が一方的に接続を切ったということなので
					// fdをcloseし、vectorから構造体を削除する
					close(c->pfd.fd);
					c = connections.erase(c);
					continue;
				}
				// 書き込みイベントが終わったら、次のリクエストメッセージの受信を待つためにeventsフィールドにPOLLINをセットする
				c->pfd.events = POLLIN;
			}
			++c;
		}
	}

	server_close(EXIT_SUCCESS, listens, connections);
	return 0;
}