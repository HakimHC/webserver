#include "Request.hpp"

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

enum requestLine { HTTP_METHOD, URI, HTTP_VERSION };

Request::Request() {}
Request::~Request() {}

void Request::parseLegacy(std::string const &buffer) {
  std::string tmp = this->getRequestLineLegacy(buffer);
  tmp = this->getRequestHeadersLegacy(tmp);
}

std::string Request::getRequestLineLegacy(std::string const &buffer) {
  size_t crLfPosition = buffer.find("\r\n");
  if (crLfPosition == std::string::npos || buffer[0] == ' ')
    throw std::runtime_error("400");

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
  if (crLfPosition == std::string::npos) throw std::runtime_error("400");
  size_t currPos = 0;
  std::string tmp;
  while (true) {
    tmp = buffer.substr(currPos, crLfPosition - currPos);
    if (tmp.empty() || tmp == "\r\n") break;
    this->_headers[tmp.substr(0, tmp.find(":"))] = tmp.substr(tmp.find(":") + 2, tmp.find("\r\n"));
    currPos = crLfPosition + 2;
    crLfPosition = buffer.find("\r\n", currPos);
  }
  return tmp;
}

void Request::parseUri(const std::string &uri) {
  size_t queryPos = uri.find("?");
  if (queryPos == std::string::npos)
    this->_uri = uri;
  else {
    this->_uri = uri.substr(0, queryPos);
    this->_queryString = uri.substr(queryPos + 1, uri.size());
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
  std::cout << "=========================" << std::endl;
}
