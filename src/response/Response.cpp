#include "Response.hpp"

Response::Response() {
  this->initStatusCodes();
};
Response::~Response() {};

void Response::initStatusCodes() {
  _statusCodes[100] = "Continue";
  _statusCodes[101] = "Switching Protocols";
  _statusCodes[200] = "OK";
  _statusCodes[201] = "Created";
  _statusCodes[202] = "Accepted";
  _statusCodes[203] = "Non-Authoritative Information";
  _statusCodes[204] = "No Content";
  _statusCodes[205] = "Reset Content";
  _statusCodes[206] = "Partial Content";
  _statusCodes[300] = "Multiple Choices";
  _statusCodes[301] = "Moved Permanently";
  _statusCodes[302] = "Found";
  _statusCodes[303] = "See Other";
  _statusCodes[304] = "Use Proxy";
  _statusCodes[305] = "Not Modified";
  _statusCodes[307] = "Temporary Redirect";
  _statusCodes[400] = "Bad Request";
  _statusCodes[401] = "Unauthorized";
  _statusCodes[402] = "Payment Required";
  _statusCodes[403] = "Forbidden";
  _statusCodes[404] = "Not Found";
  _statusCodes[405] = "Method Not Allowed";
  _statusCodes[406] = "Not Acceptable";
  _statusCodes[407] = "Proxy Authentication Required";
  _statusCodes[408] = "Request Time-out";
  _statusCodes[409] = "Conflict";
  _statusCodes[410] = "Gone";
  _statusCodes[411] = "Length Required";
  _statusCodes[412] = "Precondition Failed";
  _statusCodes[413] = "Request Entity Too Large";
  _statusCodes[414] = "Request-URI Too Large";
  _statusCodes[415] = "Unsupported Media Types";
  _statusCodes[416] = "Requested range not satisfiable";
  _statusCodes[417] = "Expectation failed";
  _statusCodes[500] = "Internal Server Error";
  _statusCodes[501] = "Not Implemented";
  _statusCodes[502] = "Bad Gateway";
  _statusCodes[503] = "Service Unavailable";
  _statusCodes[504] = "Gateway Time-out";
  _statusCodes[505] = "HTTP Version not supported";
}
