#include "echoserv.hpp"

void	server_close(int n, std::vector<Listen> &listens, std::vector<Connection> &connections) {
	for (std::vector<Listen>::iterator it = listens.begin(); it != listens.end(); ++it) {
		if (it->pfd.fd != -1) {
			close(it->pfd.fd);
			it->pfd.fd = -1;
		}
	}
	for (std::vector<Connection>::iterator it = connections.begin(); it != connections.end(); ++it) {
		if (it->pfd.fd != -1) {
			close(it->pfd.fd);
			it->pfd.fd = -1;
		}
	}
	if (n == EXIT_FAILURE) {
		std::exit(EXIT_FAILURE);
	}
}

static bool	is_num(const std::string &s) {
	for (size_t i = 0; i < s.length(); i += 1) {
		if (std::isdigit(s.at(i)) == 0) {
			std::cerr << "ERROR: 引数は整数である必要があります" << std::endl;
			return false;
		}
	}
	return true;
}

static bool	is_port(const std::string &s) {
	if (s.length() < 4 || s.length() > 5) {
		std::cerr << "ERROR: 整数の範囲は1024~49151です" << std::endl;
		return false;
	}
	int port = std::atoi(s.c_str());
	if (port < 1024 || port > 49151) {
		std::cerr << "ERROR: 整数の範囲は1024~49151です" << std::endl;
		return false;
	}
	return true;
}

bool	set_port(int argc, const char **argv ,std::vector<int> &ports) {
	if (argc == 1) {
		std::cerr << "ERROR: 引数が必要です" << std::endl;
		return false;
	}
	for (int i = 1; i < argc; i += 1) {
		if (!is_num(std::string(argv[i])) || !is_port(std::string(argv[i])))
			return false;
		ports.push_back(std::atoi(argv[i]));
	}
	return true;
}
