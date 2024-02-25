#include "Connection.hpp"

Connection::Connection(Config &config)
: _connectingTime(std::time(NULL)) , _readTime(std::time(NULL)), _cgiTime(std::time(NULL)),
_config(config), _status(NONE), _CgiReadFd(-1), _CgiReadEvent(0), _CgiReadRevent(0),
_CgiWriteFd(-1), _CgiWriteEvent(0), _CgiWriteRevent(0), _pid(0), _event(0), _revent(0),
_errStatus(0), _keep(true),
_requestAttemps(std::time(NULL), 0) {}

Connection::Connection(const Connection &src)
: _connectingTime(src._connectingTime) , _readTime(src._readTime), _cgiTime(src._cgiTime),
_config(src._config), _status(src._status), _requestInfo(src._requestInfo),
_responseInfo(src._responseInfo), _sendMessage(src._sendMessage),
_CgiReadFd(src._CgiReadFd), _CgiReadEvent(src._CgiReadEvent), _CgiReadRevent(src._CgiReadRevent),
_cgiBuf(src._cgiBuf), _CgiWriteFd(src._CgiWriteFd), _CgiWriteEvent(src._CgiWriteEvent),
_CgiWriteRevent(src._CgiWriteRevent), _cgiWriteBody(src._cgiWriteBody), _pid(src._pid), _event(src._event),
_revent(src._revent), _errStatus(src._errStatus), _keep(src._keep),
_requestAttemps(src._requestAttemps), _port(src._port), _host(src._host) {}

Connection	&Connection::operator=(const Connection &src) {
	if (this == &src) { return *this; }
	setConnectingTime(src._connectingTime);
	setReadTime(src._readTime);
	setCgiTime(src._cgiTime);
	_config = src._config;
	setConnectionStat(src._status);
	setRequestInfo(src._requestInfo);
	setResponse(src._responseInfo);
	setSendMessage(src._sendMessage);
	setCgiReadFd(src._CgiReadFd);
	setCgiReadEvent(src._CgiReadEvent);
	setCgiReadRevent(src._CgiReadRevent);
	_cgiBuf = src._cgiBuf;
	setCgiWriteFd(src._CgiWriteFd);
	setCgiWriteEvent(src._CgiWriteEvent);
	setCgiWriteRevent(src._CgiWriteRevent);
	setCgiWriteBody(src._cgiWriteBody);
	setPid(src._pid);
	setEvent(src._event);
	setRevent(src._revent);
	setErrStatus(src._errStatus);
	setKeep(src._keep);
	setRequestAttemps(src._requestAttemps);
	setPort(src._port);
	setHost(src._host);
	return *this;
}

Connection::~Connection() {}

// Config
const Config	&Connection::getConfig() const { return _config; }
Config	&Connection::getConfig() { return _config; }

// connectTime
time_t	Connection::getConnectingTime() const { return this->_connectingTime; }
void	Connection::setConnectingTime(time_t time) { _connectingTime = time; }

// ConnectionStatus
ConnectionStatus	Connection::getConnectionStat() const { return _status; }
void	Connection::setConnectionStat(ConnectionStatus status) { _status = status;}

// pollfd関連
void	Connection::setEvent(int event) { _event = event; }
void	Connection::setRevent(int revent) { _revent = revent; }
int	Connection::getEvent() const { return _event; }
int	Connection::getRevent() const { return _revent; }

// pid
pid_t	Connection::getPid() const { return _pid; }
void	Connection::setPid(pid_t pid) {_pid = pid; }
int		Connection::killPid() {
	if (getPid() == 0) { return 0;}
	if (kill(getPid(), 0) == 0) {
		std::cout << "pid kill -> [" << getPid() << "]" << std::endl;
		if (kill(getPid(), SIGTERM) == -1) {
			errMessage("kill");
			return -1;
		}
	}
	return 0;
}

// CgiSocket Read;
int	Connection::getCgiReadFd() const { return _CgiReadFd; }
void	Connection::setCgiReadFd(int cgisocket) { _CgiReadFd = cgisocket; }
void	Connection::closeCgiReadFd() {
	if (getCgiReadFd() != -1) {
		std::cout << "CgiReadSocket close -> [" << getCgiReadFd() << "]" << std::endl;
		close(getCgiReadFd());
		setCgiReadFd(-1);
		setCgiReadRevent(0);
	}
}
int	Connection::getCgiReadEvent() const { return _CgiReadEvent; }
void	Connection::setCgiReadEvent(int cgievent) { _CgiReadEvent = cgievent; }
int	Connection::getCgiReadRevent() const { return _CgiReadRevent; }
void	Connection::setCgiReadRevent(int cgirevent) { _CgiReadRevent = cgirevent; }
const std::string	&Connection::getCgiBuf() const { return _cgiBuf; }
void	Connection::addCgiBuf(const std::string &b) { _cgiBuf += b; }
void	Connection::clearCgiBuf() { _cgiBuf.clear(); }

// CgiSocket Write;
int	Connection::getCgiWriteFd() const { return _CgiWriteFd; }
void	Connection::setCgiWriteFd(int cgisocket) { _CgiWriteFd = cgisocket; }
void	Connection::closeCgiWriteFd() {
	if (getCgiWriteFd() != -1) {
		std::cout << "CgiWriteSocket close -> [" << getCgiWriteFd() << "]" << std::endl;
		close(getCgiWriteFd());
		setCgiWriteFd(-1);
		setCgiWriteRevent(0);
	}
}
int	Connection::getCgiWriteEvent() const { return _CgiWriteEvent; }
void	Connection::setCgiWriteEvent(int cgievent) { _CgiWriteEvent = cgievent; }
int	Connection::getCgiWriteRevent() const { return _CgiWriteRevent; }
void	Connection::setCgiWriteRevent(int cgirevent) { _CgiWriteRevent = cgirevent; }
const std::string	&Connection::getCgiWriteBody() const { return _cgiWriteBody; }
void	Connection::setCgiWriteBody(const std::string &b) { _cgiWriteBody = b; }
void	Connection::clearCgiWriteBody() { _cgiWriteBody.clear(); }


//errstatus
int	Connection::getErrStatus() const { return _errStatus; }
void	Connection::setErrStatus(int err) { _errStatus = err; }

// _readTime;
time_t	Connection::getReadTime() const { return _readTime; }
void			Connection::setReadTime(time_t time) { _readTime = time; }
// cgiTime;
time_t	Connection::getCgiTime() const { return _cgiTime; }
void			Connection::setCgiTime(time_t time) { _cgiTime = time; }

// RequestInfo
RequestInfo	&Connection::getRequestInfo() { return _requestInfo; }
void	Connection::setRequestInfo(const RequestInfo &info) { _requestInfo = info; }
void	Connection::clearRequestInfo() {
	_requestInfo.clearBuf();
	_requestInfo.clearLine();
	_requestInfo.clearHeader();
	_requestInfo.clearBody();
}

// keepalive close
bool	Connection::getKeep() const { return _keep; }
void	Connection::setKeep(bool f) { _keep = f; }

//responseInfo
void	Connection::setResponse(const Response &res) { _responseInfo = res; }
const Response	&Connection::getResponse() const { return _responseInfo; }
void	Connection::clearResponse() {
	_responseInfo.httpV.clear();
	_responseInfo.date.clear();
	_responseInfo.status.clear();
	_responseInfo.serverName.clear();
	_responseInfo.contentType.clear();
	_responseInfo.contentLength.clear();
	_responseInfo.lastModified.clear();
	_responseInfo.location.clear();
	_responseInfo.connection.clear();
	_responseInfo.resource.clear();
}

//sendMessage
void	Connection::setSendMessage(const std::string &res) { _sendMessage = res; }
const std::string	&Connection::getSendMessage() const { return _sendMessage; }
void	Connection::clearSendMessage() { _sendMessage.clear(); }

//std::pair<time_t, size_t>	_requestAttemps;
const std::pair<time_t, size_t>	&Connection::getRequestAttemps() const { return _requestAttemps; }
void	Connection::setRequestAttemps(const std::pair<time_t, size_t> & p) { _requestAttemps = p; }
void	Connection::incrementAttemps() {
	_requestAttemps.second = _requestAttemps.second + 1;
	}
void	Connection::resetAttemps() {
	std::pair<time_t, size_t>	p(std::time(NULL), 0);
	_requestAttemps = p;
}

//port
void	Connection::setPort(const std::string &port) { _port = port; }
const std::string	&Connection::getPort() const { return _port; }
//host
void	Connection::setHost(const std::string &host) { _host = host; }
const std::string	&Connection::getHost() const { return _host; }