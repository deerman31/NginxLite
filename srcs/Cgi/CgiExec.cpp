#include "Server.hpp"

static std::vector<std::string>	envsCreate(const ConfigServer& c, const RequestInfo& info) {
	std::vector<std::string>	envs;

	if (info.getLine().method == "POST") {
		envs.push_back("CONTENT_LENGTH=" + info.getBody().contentLength);
		envs.push_back("CONTENT_TYPE=" + info.getBody().contentType);
	}
	envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	if (!info.getHeader().accept.empty()) {
		envs.push_back("HTTP_ACCEPT=" + info.getHeader().accept);//ブラウザがサポートする Content-type: のリスト。すべてを許可する場合、 となる。
	}
	if (!info.getHeader().userAgent.empty()) {
		envs.push_back("HTTP_USER_AGENT=" + info.getHeader().userAgent); //HTTP_USER_AGENT	ブラウザに関する情報(Mozilla/4.01 [ja] (Win95; I) など)
	}
	if (info.getLine().path.find("?") == std::string::npos) {
		envs.push_back("PATH_INFO=" + info.getLine().path); //PATH_INFO	パス情報。たとえば、「cgi-bin/xxx.cgi/taro/xxx.htm」というURLでCGIスクリプトを呼び出した場合、PATH_INFOには「/taro/xxx.htm」が格納される。
		envs.push_back("SCRIPT_NAME=" + info.getLine().path.substr(info.getLine().path.rfind("/"))); // SCRIPT_NAME	CGIスクリプトの名前。
	} else {
		envs.push_back("PATH_INFO=" + info.getLine().path.substr(0, info.getLine().path.find("?")));
		envs.push_back("QUERY_STRING=" + info.getLine().path.substr(info.getLine().path.find("?") + 1));//QUERY_STRING	「http://サーバー名/CGIスクリプト名?データ」というURLを要求した場合のデータ部分。
		size_t pos1 = info.getLine().path.rfind("/");
		size_t pos2 = info.getLine().path.rfind("?");
		envs.push_back("SCRIPT_NAME=" + info.getLine().path.substr(pos1, pos2 - pos1));
	}
	envs.push_back("REQUEST_METHOD=" + info.getLine().method);//REQUEST_METHOD	METHODで指定したデータ取得手段。GET, POST, HEAD, PUT, DELETE, LINK, UNLINKなどの種類がある。 
	envs.push_back("SERVER_NAME=" + c.serverName); //SERVER_NAME	サーバー名
	envs.push_back("SERVER_PORT=" + c.port); //SERVER_PORT	サーバーのポート番号(例えば80)
	envs.push_back("SERVER_PROTOCOL=" + info.getLine().httpv); //SERVER_PROTOCOL	サーバーのプロトコル名(例えばHTTP/1.0)
	envs.push_back("SERVER_SOFTWARE=webserv"); //SERVER_SOFTWARE	サーバーのソフトウェア名(例えば NCSA/1.3)
	return envs;
}

static void	pipeClose(int *pipefd) {
	close(pipefd[0]);
	close(pipefd[1]);
}

static int	ftFcntlNonBlock(int fd) {

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		errMessage("fcntl");
		return -1;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0) {
		errMessage("fcntl");
		return -1;
	}
	return 0;
}

int	ftExec(int &fd, pid_t &pid, const std::vector<std::string> envs, const RequestInfo &info, const std::string &c1) {
	int		pipefd[2];
	if (pipe(pipefd) == -1) {
		errMessage("pipe");
		return -1;
	}
	if (ftFcntlNonBlock(pipefd[0]) < 0) {
		pipeClose(pipefd);
		return -1;
	}
	if (ftFcntlNonBlock(pipefd[1]) < 0) {
		pipeClose(pipefd);
		return -1;
	}
	pid_t retpid = fork();
	if (retpid == -1) {
		errMessage("fork");
		pipeClose(pipefd);
		return -1;
	}
	if (retpid == 0) {
		// 子プロセス
		dup2(pipefd[1], STDOUT_FILENO); // 標準出力をpipeにredirect
		dup2(pipefd[1], STDERR_FILENO); // 標準エラー出力をパイプにリダイレクト
		pipeClose(pipefd);
		// close(pipefd[0]); // 読み取り端を閉じる
		// close(pipefd[1]); // 書き込み端を閉じる
		std::vector<char*> envp;
		for (std::vector<std::string>::const_iterator it = envs.begin();
		it != envs.end(); ++it) {
			envp.push_back(const_cast<char*>(it->c_str()));
		}
		envp.push_back(NULL);
		std::string	path(info.getLine().path);
		size_t pos = path.find("?");
		if (pos != std::string::npos) {
			path = path.substr(0, pos);
		}
		std::string	execPath;
		std::string	execDir;
		if (c1.rfind("/") != std::string::npos) {
			execPath = c1.substr(c1.rfind("/") + 1);
			execDir = c1.substr(0, c1.rfind("/"));
		} else {
			execPath = c1;
			execDir = "./";
		}
		std::string	extension(path.substr(path.rfind(".") + 1));
		if (extension.find("?") != std::string::npos) {
			extension = extension.substr(0, extension.find("?"));
		}
		if (chdir(execDir.c_str()) == -1) {
			errMessage("chdir");
			std::exit(EXIT_FAILURE);
		}
		if (extension == "py") {
			char* argv[] = {(char*)"python3", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/python3", argv, envp.data());
		} else if (extension == "rb") {
			char* argv[] = {(char*)"ruby", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/ruby", argv, envp.data());
		} else if (extension == "pl") {
			char* argv[] = {(char*)"perl", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/perl", argv, envp.data());
		} else if (extension == "sh") {
			char* argv[] = {(char*)"zsh", (char*)execPath.c_str(), NULL};
			execve("/bin/zsh", argv, envp.data());
		} else {
			char* argv[] = {(char*)execPath.c_str(), NULL};
			execve(argv[0], argv, envp.data());
		}
		// execveはエラーがあった場合のみ戻る
		errMessage("execve");
		std::exit(EXIT_FAILURE);
	}
	// 親プロセス
	close(pipefd[1]);
	fd = pipefd[0];
	pid = retpid;
	return 0;
}

int	ftExecPost(int &readfd,int &writefd, pid_t &pid, const std::vector<std::string> envs, const RequestInfo &info, const std::string &c1) {
	int pipefd[2], postPipe[2];
	pid_t	retpid;

	if (pipe(pipefd) == -1) {
		errMessage("pipe");
		return -1;
	}
	if (pipe(postPipe) == -1) {
		errMessage("pipe");
		pipeClose(pipefd);
		return -1;
	}
	if (ftFcntlNonBlock(pipefd[0]) < 0) {
		pipeClose(pipefd);
		pipeClose(postPipe);
		return -1;
	}
	if (ftFcntlNonBlock(pipefd[1]) < 0) {
		pipeClose(pipefd);
		pipeClose(postPipe);
		return -1;
	}
	if (ftFcntlNonBlock(postPipe[0]) < 0) {
		pipeClose(pipefd);
		pipeClose(postPipe);
		return -1;
	}
	if (ftFcntlNonBlock(postPipe[1]) < 0) {
		pipeClose(pipefd);
		pipeClose(postPipe);
		return -1;
	}
	retpid = fork();
	if (retpid == -1) {
		errMessage("fork");
		pipeClose(pipefd);
		pipeClose(postPipe);
		return -1;
	} else if (retpid == 0) {
		dup2(pipefd[1], STDOUT_FILENO); // 標準出力のレダイレクト
		dup2(pipefd[1], STDERR_FILENO); // 標準エラー出力をパイプにリダイレクト
		dup2(postPipe[0], STDIN_FILENO); // 標準入力のリダイレクト
		pipeClose(pipefd);
		// close(pipefd[0]);
		// close(pipefd[1]);
		close(postPipe[1]);
		std::vector<char*> envp;
		for (std::vector<std::string>::const_iterator it = envs.begin();
		it != envs.end(); ++it) {
			envp.push_back(const_cast<char*>(it->c_str()));
		}
		envp.push_back(NULL);
		std::string	path(info.getLine().path);
		size_t pos = path.find("?");
		if (pos != std::string::npos) {
			path = path.substr(0, pos);
		}
		std::string	execPath;
		std::string	execDir;
		if (c1.rfind("/") != std::string::npos) {
			execPath = c1.substr(c1.rfind("/") + 1);
			execDir = c1.substr(0, c1.rfind("/"));
		} else {
			execPath = c1;
			execDir = "./";
		}
		std::string	extension(path.substr(path.rfind(".") + 1));
		if (extension.find("?") != std::string::npos) {
			extension = extension.substr(0, extension.find("?"));
		}
		if (chdir(execDir.c_str()) == -1) {
			errMessage("chdir");
			std::exit(EXIT_FAILURE);
		}
		if (extension == "py") {
			char* argv[] = {(char*)"python3", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/python3", argv, envp.data());
		} else if (extension == "rb") {
			char* argv[] = {(char*)"ruby", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/ruby", argv, envp.data());
		} else if (extension == "pl") {
			char* argv[] = {(char*)"perl", (char*)execPath.c_str(), NULL};
			execve("/usr/bin/perl", argv, envp.data());
		} else if (extension == "sh") {
			char* argv[] = {(char*)"zsh", (char*)execPath.c_str(), NULL};
			execve("/bin/zsh", argv, envp.data());
		} else {
			char* argv[] = {(char*)execPath.c_str(), NULL};
			execve(argv[0], argv, envp.data());
		}
		// execveはエラーがあった場合のみ戻る
		errMessage("execve");
		std::exit(EXIT_FAILURE);
	}

	// 親プロセス
	close(pipefd[1]); // 子プロセスの標準出力用パイプの書き込み端をクローズ
	close(postPipe[0]); // POSTデータ用パイプの読み取り端をクローズ
	readfd = pipefd[0];
	writefd = postPipe[1];

	pid = retpid;
	return 0;
}

int	Server::_CgiExec(Connection &c) {
	if (c.getConnectionStat() != CGIEXEC) {
		return 0;
	}
	const ConfigServer &servConfig = c.setConfig();
	std::vector<std::string> envs = envsCreate(servConfig, c.getRequestInfo());
	std::string	path(c.getPath(servConfig));
	if (path.find("?") != std::string::npos) {
		path = path.substr(0, path.find("?"));
	}

	int n = ftStat(path.c_str());
	if (n < 0) {
		errMessage("stat");
		c.setConnectionStat(ERROR);
		c.setErrStatus(500);
		return 0;
	}
	if (n > 2) {
		// 403 or 404
		c.setConnectionStat(ERROR);
		c.setErrStatus(n);
		return 0;
	}

	int		readfd;
	int		writefd;
	pid_t	pid;
	if (c.getRequestInfo().getLine().method == "POST") {
		if (ftExecPost(readfd, writefd, pid, envs, c.getRequestInfo(), c.getPath(servConfig)) < 0) {
			//ftExecPostの中のエラーはリクエストとは関係ないエラーのため500
			c.killPid();
			c.setConnectionStat(ERROR);
			c.setErrStatus(500);
			return 0;
		}
		c.setPid(pid);
		c.setCgiReadFd(readfd);
		c.setCgiWriteFd(writefd);
		c.setCgiWriteEvent(POLLOUT);
		c.setEvent(0);
		c.setCgiWriteBody(c.getRequestInfo().getBody().body);
		c.setConnectionStat(CGIWRITE);
		return 0;
	} else {
		if (ftExec(readfd, pid, envs, c.getRequestInfo(), c.getPath(servConfig)) < 0) {
			//ftExecの中のエラーはリクエストとは関係ないエラーのため500
			c.killPid();
			c.setConnectionStat(ERROR);
			c.setErrStatus(500);
			return 0;
		}
	}

	c.setPid(pid);
	c.setCgiReadFd(readfd);
	c.setCgiTime(std::time(NULL));

	c.setEvent(0);
	c.setConnectionStat(CGIWAIT);

	return 0;
}