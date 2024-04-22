#include "webserv.hpp"

void	errMessage(const std::string &s) {
	std::cerr << "FAILE: " << s << " (" << errno << ") " << strerror(errno) << std::endl; 
}

std::vector<std::string> ftSplit(const std::string& s, const std::string& del) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = s.find(del);

	while (end != std::string::npos) {
		tokens.push_back(s.substr(start, end - start));
		start = end + del.length();
		end = s.find(del, start);
	}
	tokens.push_back(s.substr(start, end));
	return tokens;
}

std::string	ftItoa(size_t len) {
	std::stringstream ss;
	ss << len;
	return ss.str();
}

int	ftStoi(const std::string &s) {
	int	n;
	std::istringstream ss(s);
	ss >> n;
	return n;
}

std::string	setContentsType(const std::string& s) {
	int n = ftStat(s.c_str());

	// ディレクトリということ
	if (n == 2) { return "text/html"; }

	std::string ret;
	// 拡張子がないため、text/plainを返す
	if (s.rfind(".") == std::string::npos) {
		return "application/octet-stream";
		//return "text/plain; charset=UTF-8";
	}
	//else {ret = "application/octet-stream";}

	size_t pos = s.rfind(".");

	std::string tmp = s.substr(pos);
	if (tmp == ".html" || tmp == ".htm") {
		ret = "text/html; charset=UTF-8";}
	else if (tmp == ".aac") {ret = "audio/aac";}
	else if (tmp == ".abw") {ret = "application/x-abiword";}
	else if (tmp == ".arc") {ret = "application/x-freearc";}
	else if (tmp == ".avi") {ret = "video/x-msvideo";}
	else if (tmp == ".azw") {ret = "application/vnd.amazon.ebook";}
	else if (tmp == ".bin") {ret = "application/octet-stream";}
	else if (tmp == ".bmp") {ret ="image/bmp";}
	else if (tmp == ".bz") {ret = "application/x-bzip";}
	else if (tmp == ".csh") {ret = "application/x-csh";}
	else if (tmp == ".css") {ret = "text/css";}
	else if (tmp == ".csv") {ret = "text/csv";}
	else if (tmp == ".doc") {ret =  "application/msword";}
	else if (tmp == ".docx") {ret =  "application/vnd.openxmlformats-officedocument.wordprocessingml.document";}
	else if (tmp == ".eot") {ret = "application/vnd.ms-fontobject";}
	else if (tmp == ".epub") {ret = "application/epub+zip";}
	else if (tmp == ".gz") {ret = "application/gzip";}
	else if (tmp == ".gif") {ret = "image/gif";}
	else if (tmp == ".ico") {ret = "image/vnd.microsoft.icon";}
	else if (tmp == ".ics") {ret = "text/calendar";}
	else if (tmp == ".jar") {ret = "application/java-archive";}
	else if (tmp == ".jpg" || tmp == ".jpeg") {ret = "image/jpeg";}
	else if (tmp == ".js" || tmp == "mjs") {ret = "text/javascript";}
	else if (tmp == ".json") {ret = "application/json";}
	else if (tmp == ".jsonld") {ret = "application/ld+json";}
	else if (tmp == "..mid" || tmp == "..midi") {ret = "audio/midi";} //audio/x-midi
	else if (tmp == ".mp3") {ret = "audio/mpeg";}
	else if (tmp == ".mpeg") {ret = "video/mpeg";}
	else if (tmp == ".mpkg") {ret = "application/vnd.apple.installer+xml";}
	else if (tmp == ".odp") {ret = "application/vnd.oasis.opendocument.presentation";}
	else if (tmp == ".ods") {ret = "application/vnd.oasis.opendocument.spreadsheet";}
	else if (tmp == ".odt") {ret = "application/vnd.oasis.opendocument.text";}
	else if (tmp == ".oga") {ret = "audio/ogg";}
	else if (tmp == ".ogv") {ret = "video/ogg";}
	else if (tmp == ".ogx") {ret = "application/ogg";}
	else if (tmp == ".opus") {ret = "audio/opus";}
	else if (tmp == ".otf") {ret = "font/otf";}
	else if (tmp == ".png") {ret = "image/png";}
	else if (tmp == ".pdf") {ret = "application/pdf";}
	else if (tmp == ".php") {ret = "application/x-httpd-php";}
	else if (tmp == ".ppt") {ret = "application/vnd.ms-powerpoint";}
	else if (tmp == ".pptx") {ret = "application/vnd.openxmlformats-officedocument.presentationml.presentation";}
	else if (tmp == ".rar") {ret = "application/vnd.rar";}
	else if (tmp == ".rtf") {ret = "application/rtf";}
	else if (tmp == ".sh") {ret = "application/x-sh";}
	else if (tmp == ".svg") {ret = "image/svg+xml";}
	else if (tmp == ".swf") {ret = "application/x-shockwave-flash";}
	else if (tmp == ".tar") {ret = "application/x-tar";}
	else if (tmp == ".tiff" || tmp == ".tif") {ret = "image/tiff";}
	else if (tmp == ".ts") {ret = "video/mp2t";}
	else if (tmp == ".ttf") {ret = "font/ttf";}
	else if (tmp == ".txt") {ret = "text/plain; charset=UTF-8";}
	else if (tmp == ".vsd") {ret = "application/vnd.visio";}
	else if (tmp == ".wav") {ret = "audio/wav";}
	else if (tmp == ".weba") {ret = "audio/webm";}
	else if (tmp == ".webm") {ret = "video/webm";}
	else if (tmp == ".webp") {ret = "image/webp";}
	else if (tmp == ".woff") {ret = "font/woff";}
	else if (tmp == ".woff2") {ret = "font/woff2";}
	else if (tmp == ".xhtml") {ret = "application/xhtml+xml";}
	else if (tmp == ".xls") {ret = "application/vnd.ms-excel";}
	else if (tmp == ".xlsx") {ret = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";}
	else if (tmp == ".xml") {ret = "application/xml";} //application/xml: 一般のユーザーから読めるものではない場合 (RFC 3023, section 3) text/xml: 一般のユーザーから読めるものである場合 (RFC 3023, section 3)
	else if (tmp == ".xul") {ret = "application/vnd.mozilla.xul+xml";}
	else if (tmp == ".zip") {ret = "application/zip";}
	else if (tmp == ".3gp") {ret = "video/3gpp";} //video/3gpp 動画を含まない場合は audio/3gpp
	else if (tmp == ".3g2") {ret = "video/3gpp2";} //video/3gpp2 動画を含まない場合は audio/3gpp2
	else if (tmp == ".7z") {ret = "application/x-7z-compressed";}
	else if (tmp == ".mp4") {ret = "video/mp4";}
	//else if (tmp == ".cgi") {ret = "application/octet-stream";}
	//else {ret = "text/plain; charset=UTF-8";}
	else {ret = "application/octet-stream";}

	return ret;
}

int	ftStat(const char *path) {
	struct stat path_stat;
	int stat_res = stat(path, &path_stat);

	if (stat_res == -1) {
		if (errno == ENOENT)
			return 404;
		return -1;
	}
	if (S_ISREG(path_stat.st_mode)) {
		//if (access(path, R_OK) == -1 || access(path, W_OK) == -1 || access(path, X_OK) == -1)
		if (access(path, R_OK) == -1 || access(path, W_OK) == -1)
			return 403;
		return 1;
	}
	if (S_ISDIR(path_stat.st_mode)) {
		//if (access(path, R_OK) == -1 || access(path, W_OK) == -1 || access(path, X_OK) == -1)
		if (access(path, R_OK) == -1 || access(path, W_OK) == -1)
			return 403;
		return 2;
	}
	return -1;
}