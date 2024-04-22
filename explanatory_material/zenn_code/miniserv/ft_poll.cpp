#include "echoserv.hpp"

int	ft_poll(std::vector<Listen>	&listen_fds, std::vector<Connection> &connection_fds) {
	int					ret;
	std::vector<pollfd>	tmp;

	// poll()は一つのリストでなければ使えないため、tmpに二つのvectorを追加する
	// listen_fdsの各々のpollfdをtmpに追加する
	for (std::vector<Listen>::const_iterator it = listen_fds.begin(); it != listen_fds.end(); ++it) {
		tmp.push_back(it->pfd);
	}
	// connection_fdsの各々のpollfdをtmpに追加する
	for (std::vector<Connection>::const_iterator it = connection_fds.begin(); it != connection_fds.end(); ++it) {
		tmp.push_back(it->pfd);
	}
	// 第３引数はブロックする時間 -1をセットすると永遠にブロックし続ける　0の場合はブロックせずにすぐに0を返す
	ret = poll(tmp.data(), tmp.size(), 100);
	if (ret < 0) {
		perror("poll");
		return -1;
	} else if (ret == 0) {
		// 戻り値が0ということはイベントが発生していない
		std::cout << "TIMEOUT!" << std::endl;
		return 0;
	}
	// pollを通過するとイベントが発生したpollfdのreventsフィールドにビットが立つので各vectorのreventsを更新する
	std::vector<pollfd>::const_iterator t = tmp.begin();
	for (std::vector<Listen>::iterator it = listen_fds.begin(); it != listen_fds.end(); ++it) {
		it->pfd.revents = t->revents;
		++t;
	}
	for (std::vector<Connection>::iterator it = connection_fds.begin(); it != connection_fds.end(); ++it) {
		it->pfd.revents = t->revents;
		++t;
	}
	return ret;
}