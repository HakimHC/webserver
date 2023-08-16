#include <fcntl.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <iostream>
#include <exception>

#include "Listener.hpp"
#include "Request.hpp"

Listener::Listener(uint16_t port) 
  :_port(port) {

    this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_socketFd < 0)
      throw std::runtime_error("fatal: cannot create socket ");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_socketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
      throw std::runtime_error("fatal: cannot bind socket");

    std::cout << "Server listening on 127.0.0.1:" << this->_port << "..."
      << std::endl;

    if (listen(this->_socketFd, 5) < 0)
      throw std::runtime_error("fatal: socket cannot listen");

    /* Auxiliaty client, the first element in our pollfd vector will always be the
     * servers fd, this is to make it parallel. */
    this->_clients.push_back(Client(-1));
    this->initPoll();
}

void Listener::initPoll() {
  fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
  struct pollfd serverPollFd;
  serverPollFd.fd = this->_socketFd;
  serverPollFd.events = POLLIN;
  this->_pollFds.push_back(serverPollFd);
}

void Listener::addServer(Server& s) {
  this->_servers.push_back(s);
}

const uint16_t& Listener::port() const {
  return this->_port;
}

void Listener::print() const {
  std::cout << "====LISTENER====" << std::endl;
  std::cout << "Port: " << this->_port << std::endl;
  for (size_t i = 0; i < this->_servers.size(); i++)
    this->_servers[i].print();
}

void Listener::acceptClient() {
  /* Construct client object with the file descriptor returned by accept */
  Client client(accept(this->_socketFd, NULL, NULL));

  if (client.getSocketfd() == -1) {
    std::cerr << "error: cannot accept client's connection" << std::endl;
    return;
  }
  struct pollfd clientPollFd;
  clientPollFd.fd = client.getSocketfd();
  clientPollFd.events = POLLIN;

  this->_pollFds.push_back(clientPollFd);
  this->_clients.push_back(client);
}

void Listener::readClientData(const size_t &clientIndex) {
  char buf[_MAX_BUFFER_SIZE];
  memset(buf, 0, sizeof(buf));

  ssize_t r = recv(this->_pollFds[clientIndex].fd, buf, sizeof(buf), 0);

  if (r == 0) {
    std::cout << "Client disconnected" << std::endl;
    close(this->_pollFds[clientIndex].fd);
  } else if (r == -1) {
    std::cout << "error: cannot read clients buffer, ending connection"
              << std::endl;
    close(this->_pollFds[clientIndex].fd);
  } else {
    std::cout << "Recieved data:" << std::endl;
    this->_clients[clientIndex].setRequestBuffer(buf);
    std::cout << this->_clients[clientIndex].getRequestBuffer();
    /* Request req; */
    /* req.parse(this->_clients[clientIndex].getRequestBuffer()); */
    /* req.print(); */
  }
}

void Listener::_listen() {
  if (poll(this->_pollFds.data(), this->_pollFds.size(), 0) < 0)
    throw std::runtime_error(
        "fatal: poll() syscall failed, shutting down server...");

  for (size_t i = 0; i < this->_pollFds.size(); i++) {
    if (this->_pollFds[i].revents & POLLIN) {
      if (this->_pollFds[i].fd == this->_socketFd) {
        std::cout << "Incoming connection..." << std::endl;
        this->acceptClient();
      } else {
        this->readClientData(i);
      }
    }
  }
}
