#include <stdexcept>
#include <iostream>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Server.hpp"
#include "Request.hpp"
#include "utils.hpp"

#define _BACKLOG 5

Server::~Server() {}
Server::Server() {}

void Server::initialize() {
  this->_port = 8282;
  this->_name = "server1";
  this->_root = "www";
  this->_maxClientBodySize = 4096;
  this->_defaultFileDirectory = "";
  this->_allowedMethods.push_back("GET");

  this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->_socketFd < 0) throw std::runtime_error("fatal: cannot create socket + (" + this->_name + ")");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->_port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(this->_socketFd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
    throw std::runtime_error("fatal: cannot bind socket");

  std::cout << "Server listening on 127.0.0.1:" << this->_port << "..." << std::endl;

  if (listen(this->_socketFd, _BACKLOG) < 0)
    throw std::runtime_error("fatal: socket cannot listen");

  /* Auxiliaty client, the first element in our pollfd vector will always be the servers fd, this is to make it parallel. */
  this->_clients.push_back(Client(-1));
  this->_clientBuffer.push_back("");
}

void Server::print() const {
  std::cout << std::endl;
  std::cout << "======= " << this->_name << " =======" << std::endl;
  std::cout << std::endl;
  std::cout << "Port: " << this->_port << std::endl;
  std::cout << "SockFD: " << this->_socketFd << std::endl;
  std::cout << "Root: " << this->_root << std::endl;
  std::cout << "Index: " << this->_index << std::endl;
  std::cout << "Allowed methods: ";

  std::vector<Server::HTTPMethods>::const_iterator it = this->_allowedMethods.begin();
  std::vector<Server::HTTPMethods>::const_iterator ite = this->_allowedMethods.end();

  for (; it != ite; it++) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "=======================" << std::endl;
  std::cout << std::endl;
}

void Server::acceptClient() {
  /* Construct client object with the file descriptor returned by accept */
  Client client ( accept(this->_socketFd, NULL, NULL) );

  if (client.getSocketfd() == -1) {
    std::cerr << "error: cannot accept client's connection" << std::endl;
    return;
  }
  struct pollfd clientPollFd;
  clientPollFd.fd = client.getSocketfd();
  clientPollFd.events = POLLIN;

  this->_pollFds.push_back(clientPollFd);
  this->_clients.push_back(client);
  this->_clientBuffer.push_back("");
}

void Server::readClientData(const size_t& clientIndex) {
  char buf[_MAX_BUFFER_SIZE];
  memset(buf, 0, sizeof(buf));

  ssize_t r = recv(this->_pollFds[clientIndex].fd, buf, sizeof(buf), 0);

  if (r == 0) {
    std::cout << "Client disconnected" << std::endl;
    close(this->_pollFds[clientIndex].fd);
  }
  else if  (r == -1) {
    std::cout << "error: cannot read clients buffer, ending connection" << std::endl;
    close(this->_pollFds[clientIndex].fd);
  }
  else {
    this->_clientBuffer[clientIndex] = buf;
    std::cout << this->_clientBuffer[clientIndex];
    Request req;
    /* req.parse(buf); */
    req.parse(this->_clientBuffer[clientIndex]);
    req.print();
  }
}

void Server::operate() {
  fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);

  /* Add the passive socket (server socket) to the poll-fd vector */
  struct pollfd serverPollFd;
  serverPollFd.fd = this->_socketFd;
  serverPollFd.events = POLLIN;
  this->_pollFds.push_back(serverPollFd);

  while (true) {
    if (poll(this->_pollFds.data(), this->_pollFds.size(), 0) < 0)
      throw std::runtime_error("fatal: poll() syscall failed, shutting down server...");
    
    for (size_t i = 0; i < this->_pollFds.size(); i++) {
      if (this->_pollFds[i].revents & POLLIN) {
        if (this->_pollFds[i].fd == this->_socketFd) {
          std::cout << "Incoming connection..." << std::endl;
          this->acceptClient();
        }
        else {
          std::cout << "Recieved data:" << std::endl;
          this->readClientData(i);
        }
      }
    }
  }
}
