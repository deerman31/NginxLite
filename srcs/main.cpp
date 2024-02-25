#include "webserv.hpp"

static bool argCheck(int argc, char **argv) {
	if (argc == 1) {
		return true; }
	if (argc != 2) {
		std::cout << "ERROR: There are no arguments or just one.\n";
		return false;
	}
	std::string Arg(argv[1]);
	size_t pos = Arg.rfind(".");
	if (pos == std::string::npos || pos == 0 || Arg.substr(pos) != ".conf")  {
		std::cout << "ERROR: The extension must be \".conf\".\n";
		return false;
	}
	return true;
}

static std::string	pathSet(int argc, char **argv) {
	std::string	path;
	if (argc == 1) {
		path = "conf/1.conf";
	} else {
		path = std::string(argv[1]);
	}
	return path;
}

int	main(int argc, char **argv) {
	if (!argCheck(argc, argv)) {
		return 0;
	}
	std::string	path(pathSet(argc, argv));
	Config	config;
	config.configParse(path);

	Server	server(config);
	if (server.setUp() < 0) {
		return 1;
	}
	std::signal(SIGPIPE, SIG_IGN);
	if (server.Run() < 0) {
		return 1;
	}
	return 0;
}
