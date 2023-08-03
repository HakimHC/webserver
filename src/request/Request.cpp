#include <sstream>
#include <stdexcept>
#include <iostream>

#include "Request.hpp"

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
  while (std::getline(tokenizer, token, ' ')) {
    std::cout << "TOKEN: " << token << std::endl;
  }
}
