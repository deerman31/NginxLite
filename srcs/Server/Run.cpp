#include "Server.hpp"

static void	handleSignal(int signal) {
	if (signal == SIGINT) {
		std::cout << "\nCtrl + C が押されました。無限ループを終了します。" << std::endl;
		g_stop = false;
	}
}

int	Server::Run() {
	int		ret;
	signal(SIGINT, handleSignal);
	while (g_stop) {
		_connectTimeout();
		_closeConnection();

		_CgiParse();
		_CgiWait();

		_prepairingPoll();
		ret = _pollWait();
		if (ret < 0) {
			if (errno == EINTR) { break; }
			return -1;
		}
		if (ret == 0) { continue; }

		// LisetnSocketのLoop
		for (std::map<int, Listen>::iterator l = _listens.begin();
		l != _listens.end(); ++l) {
			//ここでAcceptで新規接続
			if (_NewAccept(l->first, l->second) < 0) {
				std::cout << "Listen [" << l->first << "] 新規接続に失敗" << std::endl;
			}
		}
		// ConnectionのLoop
		for (std::map<int, Connection>::iterator c = _connects.begin();
		c != _connects.end(); ++c) {
			if (c->second.getRevent() & POLLIN) {
				// if (_listens.size() + _connects.size() > 250) {
				// 	c->second.setConnectionStat(ERROR);
				// 	c->second.setErrStatus(429);
				// }
				_inputSection(c->first, c->second);
			} else if (c->second.getRevent() & POLLOUT) {
				_Send(c->first, c->second);
			} else if (c->second.getRevent() & POLLHUP) {
				c->second.setConnectionStat(CLOSE);
			} else if (c->second.getRevent() & POLLERR) {
				c->second.setConnectionStat(CLOSE);
			} else if (c->second.getCgiReadRevent() & POLLIN) {
				_CgiRead(c->second);
			} else if (c->second.getCgiWriteRevent() & POLLOUT) {
				_CgiWrite(c->second);
			} else if (c->second.getCgiReadRevent() & POLLHUP) {
				c->second.setConnectionStat(CLOSE);
			} else if (c->second.getCgiWriteRevent() & POLLERR) {
				c->second.setConnectionStat(CLOSE);
			}
		}
		_closeConnection();
	}
	return 0;
}