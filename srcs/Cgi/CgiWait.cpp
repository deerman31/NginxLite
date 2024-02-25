#include "Server.hpp"

int	Server::_load(Connection &c) {
	if (c.getConnectionStat() != CGIWAIT) {
		return 0;
	}

	// 504 Gateway Timeout 無限ループなど一定時間以上処理に時間がかかるスクリプトである場合には５０４を返す
	if (TIMEOUT504 < difftime(std::time(NULL), c.getCgiTime())) {
		c.setConnectionStat(ERROR);
		c.closeCgiReadFd();
		c.killPid();
		c.setErrStatus(504);
		_Error(c);
		return 0;
	}
	int		status;
	pid_t	retpid;
	
	retpid = waitpid(c.getPid(), &status, WNOHANG);
	if (retpid < 0) {
		errMessage("waitpid");
		c.setConnectionStat(CLOSE);
		return -1;
	} else if (retpid == 0) {
		std::cout << "cgifd [" << c.getCgiReadFd() << "] 子プロセスはまだ終了していない" << std::endl;
		return 0;
	}
	if (WIFEXITED(status)) {
		std::cout << "cgifd [" << c.getCgiReadFd() << "] 終了ステータス: " << WEXITSTATUS(status) << std::endl;
	} else if (WIFSIGNALED(status)) {
		std::cout << "cgifd [" << c.getCgiReadFd() << "] 終了のシグナル: " << WTERMSIG(status) << std::endl;
	}
	std::cout << "子プロセスは完了した。" << std::endl;

	// 次にcgiのfdをreadしていくのでtimeをsetし、408のエラーの準備
	c.setReadTime(std::time(NULL));
	// pollにセットするためにcgiのイベントをPOLLINにする
	c.setPid(0);
	c.setCgiReadEvent(POLLIN);
	c.setCgiReadRevent(0);
	c.clearCgiBuf();
	c.setConnectionStat(CGIREAD);

	return 0;
}

int	Server::_CgiWait() {
	std::map<int, Connection>::iterator c = _connects.begin();
	for (; c != _connects.end(); ++c) {
		_load(c->second);
	}
	return 0;
}