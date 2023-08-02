#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <iostream>

#include "Server.hpp"

#define _BACKLOG 5

Server::~Server() {}
Server::Server()
  : _port(8080), _name("server1"), _root("www"), _index("index.html"),
    _maxClientBodySize(4096), _defaultFileDirectory("") {

    this->_allowedMethods.push_back(GET);

    this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketFd < 0) throw std::runtime_error("fatal: cannot create socket + (" + this->_name + ")");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
      throw std::runtime_error("fatal: cannot bind socket");

    std::cout << "Server listening on 127.0.0.1:8080" << std::endl;

    if (listen(this->_socketFd, _BACKLOG) < 0)
      throw std::runtime_error("fatal: socket cannot listen");
}

Server::Server(uint16_t port)
  : _port(port), _name("server1"), _root("www"), _index("index.html"),
    _maxClientBodySize(4096), _defaultFileDirectory("") {

    this->_allowedMethods.push_back(GET);

    this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketFd < 0) throw std::runtime_error("fatal: cannot create socket + (" + this->_name + ")");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
      throw std::runtime_error("fatal: cannot bind socket");

    std::cout << "Server listening on 127.0.0.1:8080" << std::endl;

    if (listen(this->_socketFd, _BACKLOG) < 0)
      throw std::runtime_error("fatal: socket cannot listen");
}

void Server::print() const {
  std::cout << "== " << this->_name << " ==" << std::endl;
  std::cout << "Port: " << this->_port << std::endl;
  std::cout << "SockFD: " << this->_socketFd << std::endl;
  std::cout << "Root: " << this->_root << std::endl;
  std::cout << "Index: " << this->_index << std::endl;
  std::cout << "Allowed methods: ";

  std::vector<Server::METHODS>::const_iterator it = this->_allowedMethods.begin();
  std::vector<Server::METHODS>::const_iterator ite = this->_allowedMethods.end();

  for (; it != ite; it++) {
    switch (*it) {
      case GET:
        std::cout << "GET ";
        break;
      case POST:
        std::cout << "POST ";
        break;
      case DELETE:
        std::cout << "DELETE ";
        break;
    }
  }
  std::cout << std::endl;
}
