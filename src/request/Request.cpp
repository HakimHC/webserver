#include "Request.hpp"

Request::Request() {}
Request::~Request() {}

void Request::parse(std::string const& buffer) {
  this->getRequestLine(buffer);
}

void Request::getRequestLine(std::string const& buffer) {
  (void) buffer;
}
