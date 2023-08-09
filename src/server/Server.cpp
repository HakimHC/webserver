#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "Request.hpp"
#include "utils.hpp"
#include "Location.hpp"

#define _BACKLOG 5

Server::~Server() {}
Server::Server() {}

Server::Server(std::string &serverString){
	std::istringstream iss(serverString);
		
	std::string line, s1, s2;
	while (std::getline(iss, line)) {
		Location::removeTrailing(line);
		if (find(line.begin(), line.end(), '{') != line.end()){
			std::istringstream iss2(line);
			std::getline(iss2, s1, ' ');
			if (s1 != "location")
				std::runtime_error("Unidentified object in server body");
			std::getline(iss2, s2, ' ');
			std::getline(iss, line, '}');
			Location temp(line, s2);
			_locations[temp.getUri()] = temp;
			continue ;
		}
		std::istringstream iss2(line);
		std::getline(iss2, s1, ' ');
		std::getline(iss2, s2, ';' ) ;
		if (s1 == "host"){
			this->_host = s2; 
		}
		if (s1 == "listen"){
			uint16_t	temp;
			std::istringstream iss3(s2);
			iss3 >> temp;
			if (iss3.fail())
				std::runtime_error("Incorrect parameter for listen.");
			this->_listen = temp;  
		}
		if (s1 == "server_name"){
			this->_serverName = s2; 
		}
	}
}

void Server::initialize(uint16_t port) {
  this->_port = port;
  this->_name = "server1";
  this->_root = "www";
  this->_maxClientBodySize = 4096;
  this->_defaultFileDirectory = "";
  this->_allowedMethods.push_back("GET");

  this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->_socketFd < 0)
    throw std::runtime_error("fatal: cannot create socket + (" + this->_name +
                             ")");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->_port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(this->_socketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    throw std::runtime_error("fatal: cannot bind socket");

  std::cout << "Server listening on 127.0.0.1:" << this->_port << "..."
            << std::endl;

  if (listen(this->_socketFd, _BACKLOG) < 0)
    throw std::runtime_error("fatal: socket cannot listen");

  /* Auxiliaty client, the first element in our pollfd vector will always be the
   * servers fd, this is to make it parallel. */
  this->_clients.push_back(Client(-1));

  this->initPoll();
}

void Server::initPoll() {
  fcntl(this->_socketFd, F_SETFL, O_NONBLOCK);
  struct pollfd serverPollFd;
  serverPollFd.fd = this->_socketFd;
  serverPollFd.events = POLLIN;
  this->_pollFds.push_back(serverPollFd);
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

  std::vector<Server::HTTPMethods>::const_iterator it =
      this->_allowedMethods.begin();
  std::vector<Server::HTTPMethods>::const_iterator ite =
      this->_allowedMethods.end();

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

void Server::readClientData(const size_t &clientIndex) {
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
    Request req;
    req.parse(this->_clients[clientIndex].getRequestBuffer());
    req.print();
  }
}

void Server::operate() {
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
