#include <sstream>
#include <stdexcept>
#include <iostream>

#include "Request.hpp"

enum requestLine {
  HTTP_METHOD,
  URI,
  HTTP_VERSION
};

Request::Request() {}
Request::~Request() {}

void Request::parse(std::string const& buffer) {
  this->getRequestLine(buffer);
}

void Request::getRequestLine(std::string const& buffer) {
  size_t crLfPosition = buffer.find("\r\n");
  if (crLfPosition == std::string::npos) throw std::runtime_error("400 bad request (no crlf)");

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
}

void Request::parseUri(const std::string& uri) {
  this->_uri = uri;
}

void Request::print() const {
  std::cout << "======== REQUEST ========" << std::endl;
  std::cout << "Method: " << this->_method << std::endl;
  std::cout << "URI: " << this->_uri << std::endl;
  std::cout << "Version: " << this->_httpVersion << std::endl;
  std::cout << "=========================" << std::endl;
}
