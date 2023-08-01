#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <iostream>

#include "Server.hpp"

#define _BACKLOG 5

Server::Server()
  : _port(8181), _name("server1"), _root("www"), _index("index.html") {

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

    std::cout << "Server listening on 127.0.0.1:8181" << std::endl;

    if (listen(this->_socketFd, _BACKLOG) < 0)
      throw std::runtime_error("fatal: socket cannot listen");
}

Server::~Server() {}
