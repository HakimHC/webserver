#include "Request.hpp"
#include "logging.hpp"

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

enum requestLine { HTTP_METHOD, URI, HTTP_VERSION };

Request::~Request() {}
Request::Request() {}

Request::Request(const std::string& requestBuffer) {
  this->parseLegacy(requestBuffer);
}

void Request::parseLegacy(std::string const& buffer) {
  std::string tmp = this->getRequestLineLegacy(buffer);
  tmp = this->getRequestHeadersLegacy(tmp);
  this->getRequestBodyLegacy(tmp);
}

std::string Request::getRequestLineLegacy(std::string const& buffer) {
  size_t crLfPosition = buffer.find("\r\n");
  if (crLfPosition == std::string::npos || buffer[0] == ' ')
    throw std::runtime_error("400 Bad Request");

  std::string line = buffer.substr(0, crLfPosition);
  std::stringstream tokenizer(line);
  std::string token;
  int8_t i = 0;
  while (std::getline(tokenizer, token, ' ')) {
    if (!token.empty()) {
      switch (i) {
        case HTTP_METHOD:
          this->_method = token;
          break;
        case URI:
          this->parseUri(token);
          break;
        case HTTP_VERSION:
          this->_httpVersion = token;
      }
      ++i;
    }
  }
  return buffer.substr(crLfPosition + 2, buffer.size());
}

std::string Request::getRequestHeadersLegacy(const std::string& buffer) {
  size_t crLfPosition = buffer.find("\r\n");
  if (crLfPosition == std::string::npos) throw std::runtime_error("400 Bad Request");
  size_t currPos = 0;
  std::string tmp;
  while (true) {
    tmp = buffer.substr(currPos, crLfPosition - currPos);
    if (tmp.empty() || tmp == "\r\n") break;
    this->_headers[tmp.substr(0, tmp.find(":"))] =
        tmp.substr(tmp.find(":") + 2, tmp.find("\r\n"));
    currPos = crLfPosition + 2;
    crLfPosition = buffer.find("\r\n", currPos);
  }
  return buffer.substr(crLfPosition + 2, buffer.size());
}

void Request::parseUri(const std::string& uri) {
  size_t queryPos = uri.find("?");
  if (queryPos == std::string::npos)
    this->_uri = uri;
  else {
    this->_uri = uri.substr(0, queryPos);
    this->_queryString = uri.substr(queryPos + 1, uri.size());
  }
}

void Request::getRequestBodyLegacy(const std::string& buffer) {
  if (buffer.size() >= 2) {
    std::string tmp = buffer.substr(2, buffer.size());
    this->_body = buffer;
  }
}

const std::map<std::string, std::string>& Request::headers() const {
  return this->_headers;
}

void Request::print() const {
  std::cout << "======== REQUEST ========" << std::endl;
  std::cout << "Method: " << this->_method << std::endl;
  std::cout << "URI: " << this->_uri << std::endl;
  std::cout << "Query String: " << this->_queryString << std::endl;
  std::cout << "Version: " << this->_httpVersion << std::endl;
  std::cout << "HEADERS: ";
  this->printHeaders();
  std::cout << "BODY: " << this->_body << std::endl;
  std::cout << "=========================" << std::endl;
}

void Request::printHeaders() const {
  std::map<std::string, std::string>::const_iterator it = this->_headers.begin();
  std::map<std::string, std::string>::const_iterator ite = this->_headers.end();

  for (; it != ite; it++) std::cout << it->first << ": " << it->second << std::endl;
}

requestType Request::convertRequestType(std::string const& method) {
  log("converting.... METHOD: " << method);
  if (method == "GET") return GET;
  if (method == "POST") return POST;
  if (method == "DELETE") return DELETE;
  return UNKNOWN;
}

const std::string& Request::getMethod() const { return this->_method; }
const std::string& Request::getUri() const { return this->_uri; }
const std::string& Request::getHttpVersion() const { return this->_httpVersion;}
const std::string& Request::getQueryString() const { return this->_queryString; }
const std::string& Request::getBody() const { return this->_body; }
const std::string& Request::getResource() const { return this->_resource; }
const std::string& Request::getLocation() const { return this->_location; }
const std::map<std::string, std::string>& Request::getHeaders() const { return this->_headers; }

void Request::setMethod(const std::string& s) { this->_method = s;}
void Request::setUri(const std::string& s) { this->_uri = s;}
void Request::setHttpVersion(const std::string& s) { this->_httpVersion = s;}
void Request::setQueryString(const std::string& s) { this->_queryString = s;}
void Request::setBody(const std::string& s) { this->_body = s;}
void Request::setResource(const std::string& s) { this->_resource = s;}
void Request::setLocation(const std::string& s) { this->_location = s;}
void Request::setHeaders(const std::map<std::string, std::string>& s) { this->_headers = s; }
