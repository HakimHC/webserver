#include "Client.hpp"

Client::~Client() {}

Client::Client(int fd) : _fd(fd) {}

void Client::setSocketFd(const int &fd) { this->_fd = fd; }
const int &Client::getSocketfd() const { return this->_fd; }
void Client::setRequestBuffer(const std::string &s) {
  this->_requestBuffer = s;
}
const std::string &Client::getRequestBuffer() const {
  return this->_requestBuffer;
}
