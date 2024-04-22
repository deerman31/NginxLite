#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

//#include <netdb.h>
#include <stack>
#include <unistd.h>
#include <sys/stat.h>

struct LocationElement {
	size_t			root;
	size_t			index;
	size_t			autoindex;
	size_t			cgi;
	size_t			allowmethods;
	size_t			redirect;
	size_t			upload;
};
struct ServerElement {
	size_t							port;
	size_t							host;
	size_t							root;
	size_t							serverName;
	size_t							errorPage;
	size_t							bodySize;
	std::vector<LocationElement>	locaElements;
};

struct Location {
	std::string path;
	std::vector<std::string> allowMethods;
	std::string redirect;
	std::string root;
	bool autoIndex;
	std::vector<std::string> indexFiles;
	std::vector<std::string> cgiHandler;
	std::string uploadPath;
};

struct ConfigServer {
	std::string	root;
	std::string host;
	std::string	port;
	std::string serverName;
	bool defaultServer;
	std::map<int, std::string> errorPages;
	size_t limitBodySize;
	std::vector<Location> locations;
};

class Config {
	private:
	std::vector<ConfigServer> _servers;

	bool	getConfigFile(const std::string &, std::string &);
	bool	isConfig(const std::string &);
	std::vector<std::string>	configSplit(const std::string &);
	std::vector<std::string>	delSplit(const std::string &, char);
	void	headendTrim(std::vector<std::string> &);
	ConfigServer	servCreate(const std::vector<std::string> &);
	void	defaultServSet();

	void	errorPageCreate(std::map<int, std::string> &, const std::string &);
	Location	locationCreate(const std::vector<std::string> &, const std::string &);

	public:
	Config();
	~Config();
	Config(const Config &);
	Config	&operator=(const Config &);

	const std::vector<ConfigServer>	&getServers() const;

	int	configParse(const std::string &);

	void	printConfig();

};

const std::string _host = "host ";
const std::string _port = "port ";
const std::string _servname = "server_name ";
const std::string _errorpage = "error_page ";
const std::string _client_max_body_size = "client_max_body_size ";
const std::string _location = "location ";

const std::string _methos = "allow_methods ";
const std::string _redirect = "redirect ";
const std::string _root = "root ";
const std::string _autoindex = "autoindex on";
const std::string _index = "index ";
const std::string _cgi = "cgi ";
const std::string _upload = "upload ";

std::vector<std::string> ftSplit(const std::string& s, const std::string& del);
void	errMessage(const std::string &);
int		ftStat(const char *);

#endif
