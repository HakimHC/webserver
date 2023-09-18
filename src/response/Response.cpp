#include <sstream>

#include "Response.hpp"
#include "logging.hpp"

Response::~Response(){
	if (this->_cgi != NULL){
		delete _cgi;
		_cgi = NULL;
	}
}
Response::Response(): _cgi(NULL) {
  this->initStatusCodes();
  this->initMimeTypes();
  this->generateCurrentDateTime();
  this->initHeaders();
}

Response::Response(int sc) : _responseStatusCode(sc), _cgi(NULL) {
  this->initStatusCodes();
  this->generateCurrentDateTime();
  std::stringstream statusCodeString;
  statusCodeString << this->_responseStatusCode;
  this->_body += "<html><head><title>" + statusCodeString.str() + " " +
                 this->_statusCodesMap[sc];
  this->_body += "</title></head><body><h1><center>" + statusCodeString.str() +
                 " " + this->_statusCodesMap[sc];
  this->_body += "</center></h1></body></html>\n";
  std::stringstream ss;
  ss << this->_body.size();
  this->_headers["Content-Length"] = ss.str();
  this->_headers["Content-Type"] = "text/html";
  this->_headers["Server"] = "adrgonza-warriors-server";
  this->generateResponseData();
}

Response::Response(const std::vector<std::string> &dirContents): _cgi(NULL) {
  this->initStatusCodes();
  this->generateCurrentDateTime();
  this->_extension = ".html";
  this->_responseStatusCode = 200;

  std::string encodedDirectoryName = dirContents[0];

  this->_body +=
      "<html><head><title>Index of " + encodedDirectoryName + "</title></head>";
  this->_body += "<body><h1>Index of " + encodedDirectoryName +
                 "</h1><hr><pre><a href=\"../\">../</a>\n";
  for (size_t i = 0; i < encodedDirectoryName.length(); i++) {
    if (encodedDirectoryName[i] == '<') {
      encodedDirectoryName.replace(i, 1, "%3C");
      i += 2;
    } else if (encodedDirectoryName[i] == '>') {
      encodedDirectoryName.replace(i, 1, "%3E");
      i += 2;
    } else if (encodedDirectoryName[i] == '/') {
      encodedDirectoryName.replace(i, 1, "%2F");
      i += 2;
    }
  }
  for (size_t i = 1; i < dirContents.size(); i++) {
    std::string encodedFileName = dirContents[i];
    for (size_t j = 0; j < encodedFileName.length(); j++) {
      if (encodedFileName[j] == '<') {
        encodedFileName.replace(j, 1, "%3C");
        j += 2;
      } else if (encodedFileName[j] == '>') {
        encodedFileName.replace(j, 1, "%3E");
        j += 2;
      } else if (encodedFileName[j] == '/') {
        encodedFileName.replace(j, 1, "%2F");
        j += 2;
      }
    }
    this->_body +=
        "<a href=\"" + encodedFileName + "\">" + encodedFileName + "</a>\n";
  }
  this->_body += "<pre><hr></body></html>";
  this->initHeaders();
  this->generateResponseData();
}

void Response::generateResponseData() {
  std::map<std::string, std::string>::iterator it = this->_headers.begin();
  std::map<std::string, std::string>::iterator ite = this->_headers.end();

  std::stringstream ss;
  ss << this->_responseStatusCode;
  this->_allData = "";
  this->_allData += std::string(HTTP_VERSION) + " " + ss.str() + " " +
                    this->_statusCodesMap[this->_responseStatusCode] + "\r\n";
  for (; it != ite; it++)
    this->_allData += it->first + ": " + it->second + "\r\n";
  this->_allData += "\r\n";
  if (this->_responseStatusCode != 204)
    this->_allData += this->_body;
}

const std::string &Response::getData() const { return this->_allData; }

void Response::setResponseStatusCode(const int &statusCode) {
  this->_responseStatusCode = statusCode;
}

void Response::initHeaders() {
  std::stringstream ss;
  ss << this->_body.size();
  this->_headers["Content-Length"] = ss.str();
  this->_headers["Content-Type"] = this->_mimeTypesMap[this->_extension];
  this->_headers["Server"] = "adrgonza-warriors-server";
}

void Response::generateCurrentDateTime() {
  time_t currentTime;

  struct tm *timeInfo;
  char timeBuffer[80];

  time(&currentTime);
  timeInfo = gmtime(&currentTime);

  strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %b %Y %H:%M:%S GMT",
           timeInfo);

  this->_headers["Date"] = timeBuffer;
}

void Response::setBody(const std::string &s) { this->_body = s; }

void Response::initStatusCodes() {
  _statusCodesMap[100] = "Continue";
  _statusCodesMap[101] = "Switching Protocols";
  _statusCodesMap[200] = "OK";
  _statusCodesMap[201] = "Created";
  _statusCodesMap[202] = "Accepted";
  _statusCodesMap[203] = "Non-Authoritative Information";
  _statusCodesMap[204] = "No Content";
  _statusCodesMap[205] = "Reset Content";
  _statusCodesMap[206] = "Partial Content";
  _statusCodesMap[300] = "Multiple Choices";
  _statusCodesMap[301] = "Moved Permanently";
  _statusCodesMap[302] = "Found";
  _statusCodesMap[303] = "See Other";
  _statusCodesMap[304] = "Use Proxy";
  _statusCodesMap[305] = "Not Modified";
  _statusCodesMap[307] = "Temporary Redirect";
  _statusCodesMap[400] = "Bad Request";
  _statusCodesMap[401] = "Unauthorized";
  _statusCodesMap[402] = "Payment Required";
  _statusCodesMap[403] = "Forbidden";
  _statusCodesMap[404] = "Not Found";
  _statusCodesMap[405] = "Method Not Allowed";
  _statusCodesMap[406] = "Not Acceptable";
  _statusCodesMap[407] = "Proxy Authentication Required";
  _statusCodesMap[408] = "Request Time-out";
  _statusCodesMap[409] = "Conflict";
  _statusCodesMap[410] = "Gone";
  _statusCodesMap[411] = "Length Required";
  _statusCodesMap[412] = "Precondition Failed";
  _statusCodesMap[413] = "Request Entity Too Large";
  _statusCodesMap[414] = "Request-URI Too Large";
  _statusCodesMap[415] = "Unsupported Media Types";
  _statusCodesMap[416] = "Requested range not satisfiable";
  _statusCodesMap[417] = "Expectation failed";
  _statusCodesMap[500] = "Internal Server Error";
  _statusCodesMap[501] = "Not Implemented";
  _statusCodesMap[502] = "Bad Gateway";
  _statusCodesMap[503] = "Service Unavailable";
  _statusCodesMap[504] = "Gateway Time-out";
  _statusCodesMap[505] = "HTTP Version not supported";
}

void Response::addHeader(const std::string &key, const std::string &value) {
  this->_headers[key] = value;
}

void Response::setErrorPageFile(const int &key, const std::string &value) {
  this->_errorPageFiles[key] = value;
}

void Response::initMimeTypes() {
  _mimeTypesMap[""] = "text/plain";
  _mimeTypesMap[".aac"] = "audio/aac";
  _mimeTypesMap[".abw"] = "application/x-abiword";
  _mimeTypesMap[".arc"] = "application/x-freearc";
  _mimeTypesMap[".avif"] = "image/avif";
  _mimeTypesMap[".avi"] = "video/x-msvideo";
  _mimeTypesMap[".azw"] = "application/vnd.amazon.ebook";
  _mimeTypesMap[".bin"] = "application/octet-stream";
  _mimeTypesMap[".bmp"] = "image/bmp";
  _mimeTypesMap[".bz"] = "application/x-bzip";
  _mimeTypesMap[".bz2"] = "application/x-bzip2";
  _mimeTypesMap[".cda"] = "application/x-cdf";
  _mimeTypesMap[".csh"] = "application/x-csh";
  _mimeTypesMap[".css"] = "text/css";
  _mimeTypesMap[".csv"] = "text/csv";
  _mimeTypesMap[".doc"] = "application/msword";
  _mimeTypesMap[".docx"] =
      "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
  _mimeTypesMap[".eot"] = "application/vnd.ms-fontobject";
  _mimeTypesMap[".epub"] = "application/epub+zip";
  _mimeTypesMap[".gz"] = "application/gzip";
  _mimeTypesMap[".gif"] = "image/gif";
  _mimeTypesMap[".htm"] = "text/html";
  _mimeTypesMap[".html"] = "text/html";
  _mimeTypesMap[".ico"] = "image/vnd.microsoft.icon";
  _mimeTypesMap[".ics"] = "text/calendar";
  _mimeTypesMap[".jar"] = "application/java-archive";
  _mimeTypesMap[".jpeg"] = "image/jpeg";
  _mimeTypesMap[".jpg"] = "image/jpeg";
  _mimeTypesMap[".js"] = "text/javascript";
  _mimeTypesMap[".json"] = "application/json";
  _mimeTypesMap[".jsonld"] = "application/ld+json";
  _mimeTypesMap[".mid"] = "audio/midi";
  _mimeTypesMap[".midi"] = "audio/midi";
  _mimeTypesMap[".mjs"] = "text/javascript";
  _mimeTypesMap[".mp3"] = "audio/mpeg";
  _mimeTypesMap[".mp4"] = "video/mp4";
  _mimeTypesMap[".mpeg"] = "video/mpeg";
  _mimeTypesMap[".mpkg"] = "application/vnd.apple.installer+xml";
  _mimeTypesMap[".odp"] = "application/vnd.oasis.opendocument.presentation";
  _mimeTypesMap[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
  _mimeTypesMap[".odt"] = "application/vnd.oasis.opendocument.text";
  _mimeTypesMap[".oga"] = "audio/ogg";
  _mimeTypesMap[".ogv"] = "video/ogg";
  _mimeTypesMap[".ogx"] = "application/ogg";
  _mimeTypesMap[".opus"] = "audio/opus";
  _mimeTypesMap[".otf"] = "font/otf";
  _mimeTypesMap[".png"] = "image/png";
  _mimeTypesMap[".pdf"] = "application/pdf";
  _mimeTypesMap[".php"] = "application/x-httpd-php";
  _mimeTypesMap[".ppt"] = "application/vnd.ms-powerpoint";
  _mimeTypesMap[".pptx"] =
      "application/"
      "vnd.openxmlformats-officedocument.presentationml.presentation";
  _mimeTypesMap[".rar"] = "application/vnd.rar";
  _mimeTypesMap[".rtf"] = "application/rtf";
  _mimeTypesMap[".sh"] = "application/x-sh";
  _mimeTypesMap[".svg"] = "image/svg+xml";
  _mimeTypesMap[".tar"] = "application/x-tar";
  _mimeTypesMap[".tif"] = "image/tiff";
  _mimeTypesMap[".tiff"] = "image/tiff";
  _mimeTypesMap[".ts"] = "video/mp2t";
  _mimeTypesMap[".ttf"] = "font/ttf";
  _mimeTypesMap[".txt"] = "text/plain";
  _mimeTypesMap[".vsd"] = "application/vnd.visio";
  _mimeTypesMap[".wav"] = "audio/wav";
  _mimeTypesMap[".weba"] = "audio/webm";
  _mimeTypesMap[".webm"] = "video/webm";
  _mimeTypesMap[".webp"] = "image/webp";
  _mimeTypesMap[".woff"] = "font/woff";
  _mimeTypesMap[".woff2"] = "font/woff2";
  _mimeTypesMap[".xhtml"] = "application/xhtml+xml";
  _mimeTypesMap[".xls"] = "application/vnd.ms-excel";
  _mimeTypesMap[".xlsx"] =
      "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
  _mimeTypesMap[".xml"] = "application/xml";
  _mimeTypesMap[".xul"] = "application/vnd.mozilla.xul+xml";
  _mimeTypesMap[".zip"] = "application/zip";
  _mimeTypesMap[".3gp"] = "video/3gpp";
  _mimeTypesMap[".3g2"] = "video/3gpp2";
  _mimeTypesMap[".7z"] = "application/x-7z-compressed";
}

void Response::setExtension(const std::string &e) { this->_extension = e; }

void Response::print() const {
  std::map<std::string, std::string>::const_iterator it = this->_headers.begin();
  std::map<std::string, std::string>::const_iterator ite = this->_headers.end();

  log(this->_allData.substr(0, this->_allData.find("\r\n")));
  for (; it != ite; it++)
    log(it->first << ": " << it->second);
  if (this->_body.size() > 4000)
    { log("Body exceeds 4000 bytes, occulting output..."); return; }
  log("\r\n" << this->_body);
}

void Response::prepareCGIResponse(){
	if (_cgi && _cgi->getResult().length() > 0){
		std::vector<std::string> separeted = CGI::separatePyCGI(_cgi->getResult());
		//do smtg
		this->setExtension(".cgi");
		this->setBody(separeted[2]);
		this->addHeader(separeted[0], separeted[1]);
		this->setResponseStatusCode(200);
		this->initHeaders();
		this->generateResponseData();
		return ;
	} else {
    	log("CGI  failed [" << _cgi->getResourcePath() << "] (" << strerror(errno) << ")");
    	if (errno == ENOENT)
      		*this = Response(404);
    	else
      		*this = Response(403);
    	return ;
	}
}

void Response::startTimer(){
	_cgiStartTime = std::time(0);
}

std::time_t Response::getCGITime(){
	return _cgiStartTime;
}