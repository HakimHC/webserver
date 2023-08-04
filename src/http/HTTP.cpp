#include "HTTP.hpp"

HTTP::HTTP() {};
HTTP::~HTTP() {};

void HTTP::addServer(Server& server) {
  this->_servers.push_back(server);
}

void HTTP::start() {
  while (true) {
    for (size_t i = 0; i < this->_servers.size(); i++)
      this->_servers[i].operate();
  }
}
