#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"
#include "Connection.hpp"

#include "Listen.hpp"
#include <vector>

#include <dirent.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_CLIENT 200
#define BUF_SIZE 10000
#define TIMEOUT408 10
#define TIMEOUT504 10
#define MAX_URL 8000
#define MAX_HEADER_SIZE 1024

static bool	g_stop = true;

// class Listen;
// class Connection;

class Server {
	private:
	std::vector<pollfd>			_fds;
	std::map<int, Listen>		_listens;
	std::map<int, Connection>	_connects;
	Config						&_config;

	int	_connectTimeout();
	int	_closeConnection();
	int	_prepairingPoll();
	int	_pollWait();
	int	_updateSocketStatus(int);
	int	_NewAccept(int, const Listen &);
	int	_inputSection(int, Connection &);
	int	_Recv(int, Connection &);
	int	_Parse(Connection &);
	int	_Method(Connection &);
	int	_Get(Connection &);
	int	_Post(Connection &);
	int	_Delete(Connection &);
	int	_CgiExec(Connection &);
	int	_CgiWait();
	int	_load(Connection &);
	int	_CgiRead(Connection &);
	int	_CgiWrite(Connection &);
	int	_CgiParse();
	int	_cgiResponseCreate(Connection &);
	int	_Redirect(Connection &);
	int	_Error(Connection &);
	std::string	_defaultErrorHtml(Connection &, int, const ConfigServer &);
	int	_Send(int, Connection &);
	bool		_isMediaType(const std::string &);
	std::string	_getLastModified(const std::string &);
	std::string	_getRFC1123Date();
	bool _readFileContents(std::string &, const std::string &);
	std::string	_listDirContents(const std::string &, const Location &);
	void _replaceAll(std::string &, const std::string &, const std::string &);

	public:
	Server(Config &config);
	~Server();
	int	setUp();
	int	Run();
};

std::vector<std::string> ftSplit(const std::string &, const std::string &);
std::string	ftItoa(size_t);
int	ftStoi(const std::string &);
std::string	setContentsType(const std::string &);
int	ftStat(const char *);
const std::string	resStatusCode(int);

#endif
