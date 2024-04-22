#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Config.hpp"
#include "RequestInfo.hpp"
#include "Response.hpp"

#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

#define CONNECTING_TIMEOUT 65
#define REQUESTLIMITS 1000

enum ConnectionStatus {
	ERROR = -1,
	NONE,
	CLOSE,
	RECV,
	PARSE,
	GET,
	POST,
	DELETE,
	REDIRECT,
	SEND,
	CGIEXEC,
	CGIWAIT,
	CGIREAD,
	CGIWRITE,
	CGIPARSE,
};

class Connection {
	private:
	time_t				_connectingTime;
	time_t				_readTime;
	time_t				_cgiTime;
	Config				&_config;
	ConnectionStatus	_status;
	RequestInfo			_requestInfo;
	Response			_responseInfo;
	std::string			_sendMessage;
	int					_CgiReadFd;
	int					_CgiReadEvent;
	int					_CgiReadRevent;
	std::string			_cgiBuf;
	int					_CgiWriteFd;
	int					_CgiWriteEvent;
	int					_CgiWriteRevent;
	std::string			_cgiWriteBody;
	pid_t				_pid;
	int					_event;
	int					_revent;
	int					_errStatus;
	bool				_keep;

	std::pair<time_t, size_t>	_requestAttemps;

	std::string			_port;
	std::string			_host;

	public:
	Connection(Config &config);
	Connection(const Connection &);
	Connection	&operator=(const Connection &);
	~Connection();

	// Config
	const Config	&getConfig() const;
	Config	&getConfig();

	// connectTime
	time_t	getConnectingTime() const;
	void			setConnectingTime(time_t time);

	// ConnectionStatus
	ConnectionStatus	getConnectionStat() const;
	void	setConnectionStat(ConnectionStatus status);

	// pollfd関連
	void	setEvent(int);
	void	setRevent(int);
	int		getEvent() const;
	int		getRevent() const;

	// pid
	pid_t	getPid() const;
	void	setPid(pid_t pid);
	int		killPid();

	// CgiSocket Read;
	int	getCgiReadFd() const;
	void	setCgiReadFd(int);
	void	closeCgiReadFd();
	int	getCgiReadEvent() const;
	void	setCgiReadEvent(int);
	int	getCgiReadRevent() const;
	void	setCgiReadRevent(int);

	const std::string	&getCgiBuf() const;

	void	addCgiBuf(const std::string &);

	void	clearCgiBuf();
	// CgiSocket Write;
	int	getCgiWriteFd() const;
	void	setCgiWriteFd(int);
	void	closeCgiWriteFd();
	int	getCgiWriteEvent() const;
	void	setCgiWriteEvent(int);
	int	getCgiWriteRevent() const;
	void	setCgiWriteRevent(int);

	const std::string	&getCgiWriteBody() const;

	void	setCgiWriteBody(const std::string &);
	void	clearCgiWriteBody();



	// errStatus;
	int	getErrStatus() const;
	void	setErrStatus(int);


	// _readTime;
	time_t	getReadTime() const;
	void			setReadTime(time_t time);
	// cgiTime;
	time_t	getCgiTime() const;
	void			setCgiTime(time_t time);

	// RequestInfo
	RequestInfo	&getRequestInfo();
	void	setRequestInfo(const RequestInfo &);
	void	clearRequestInfo();

	// keepalive close
	bool	getKeep() const;
	void	setKeep(bool);

	//responseInfo
	void	setResponse(const Response &);
	const Response	&getResponse() const;
	void	clearResponse();

	//sendMessage
	void	setSendMessage(const std::string &);
	const std::string	&getSendMessage() const;
	void	clearSendMessage();

	//std::pair<time_t, size_t>	_requestAttemps;
	const std::pair<time_t, size_t>	&getRequestAttemps() const;
	void	setRequestAttemps(const std::pair<time_t, size_t> &);
	void	incrementAttemps();
	void	resetAttemps();

	//port
	void	setPort(const std::string &port);
	const std::string	&getPort() const;
	//host
	void	setHost(const std::string &host);
	const std::string	&getHost() const;


	const std::string	getPath(const ConfigServer &);
	const ConfigServer	&setConfig();
	const Location		&nowLocation(const ConfigServer &);
};

void	errMessage(const std::string &s);

#endif
