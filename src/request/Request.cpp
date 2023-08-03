#include <exception>
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
  try {
    this->getRequestLine(buffer);
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
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
  size_t queryPos = uri.find("?");
  if (queryPos == std::string::npos) this->_uri = uri;
  else {
    this->_uri = uri.substr(0, queryPos);
    this->_queryString = uri.substr(queryPos + 1, uri.size());
  }
}

void Request::print() const {
  std::cout << "======== REQUEST ========" << std::endl;
  std::cout << "Method: " << this->_method << std::endl;
  std::cout << "URI: " << this->_uri << std::endl;
  std::cout << "Query String: " << this->_queryString << std::endl;
  std::cout << "Version: " << this->_httpVersion << std::endl;
  std::cout << "=========================" << std::endl;
}
