#include <sstream>

#include "Response.hpp"
#include "logging.hpp"

Response::~Response() {}
Response::Response() {
  this->initStatusCodes();
  this->generateCurrentDateTime();
  this->_headers["Server"] = "adrgonza-warriors-server";
}

Response::Response(int sc): _responseStatusCode(sc) {
  this->initStatusCodes();
  this->generateCurrentDateTime();
  this->_headers["Server"] = "adrgonza-warriors-server";
  std::stringstream statusCodeString;
  statusCodeString << this->_responseStatusCode;
  this->_body += "<html><head><title>" + statusCodeString.str() + " " + this->_statusCodesMap[sc];
  this->_body += "</title></head><body><h1><center>" + statusCodeString.str() + " " + this->_statusCodesMap[sc];
  this->_body += "</center></h1></body></html>\n";
  std::stringstream ss;
  ss << this->_body.size();
  this->_headers["Content-Length"] = ss.str();
  this->_headers["Content-Type"] = "text/html";
  this->generateResponseData();
}

void Response::generateResponseData() {
  this->_allData = "";
  std::stringstream ss;
  ss << this->_responseStatusCode;
  this->_allData += std::string(HTTP_VERSION) + " " + ss.str() + " " + this->_statusCodesMap[this->_responseStatusCode] + "\r\n";
  this->_allData += "Server: " + this->_headers["Server"] + "\r\n";
  this->_allData += "Content-Length: " + this->_headers["Content-Length"] + "\r\n";
  this->_allData += "Content-Type: " + this->_headers["Content-Type"] + "\r\n";
  this->_allData += "Date: " + this->_headers["Date"] + "\r\n";
  if (this->_headers.find("Location") != this->_headers.end())
    this->_allData += "Location: " + this->_headers["Location"] + "\r\n";
  this->_allData += "\r\n";
  this->_allData += this->_body;
}

const std::string& Response::getData() const {
  return this->_allData;
}

void Response::setResponseStatusCode(const int& statusCode) {
  this->_responseStatusCode = statusCode;
}

void Response::initHeaders() {
  std::stringstream ss;
  ss << this->_body.size();
  this->_headers["Content-Length"] = ss.str();
  this->_headers["Content-Type"] = "text/html";
}

void Response::generateCurrentDateTime() {
  time_t currentTime;

  struct tm* timeInfo;
  char timeBuffer[80];

  time(&currentTime);
  timeInfo = gmtime(&currentTime);

  strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

  this->_headers["Date"] = timeBuffer;
}

void Response::setBody(const std::string& s) {
  this->_body = s;
}


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

void Response::addHeader(const std::string& key, const std::string& value) {
  this->_headers[key] = value;
}
