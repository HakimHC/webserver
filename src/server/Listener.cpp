#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "Listener.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "defaults.hpp"
#include "logging.hpp"

Listener::~Listener() {}
Listener::Listener(uint16_t port) : _port(port) {

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

  if (listen(this->_socketFd, _MAX_CLIENTS) < 0)
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

void Listener::addServer(Server &s) { this->_servers.push_back(s);
checkValid(); }

const uint16_t &Listener::port() const { return this->_port; }

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
  fcntl(client.getSocketfd(), F_SETFL, O_NONBLOCK);
  struct pollfd clientPollFd;
  clientPollFd.fd = client.getSocketfd();
  clientPollFd.events = POLLIN | POLLOUT;

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
    log(this->_clients[clientIndex].getRequestBuffer());

    this->respond(this->_clients[clientIndex]);
  }
}

void Listener::_listen() {
  if (poll(this->_pollFds.data(),
           static_cast<unsigned int>(this->_pollFds.size()), 0) < 0)
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
      if (this->_clients[i].getResponse() && this->_pollFds[i].revents & POLLOUT
	  	&& this->_clients[i].getResponse()->getCGI() == NULL) {
        Client& client = this->_clients[i];
        const Response* r = client.getResponse();
        send(client.getSocketfd(), r->getData().data(), r->getData().size(), 0);
        client.setResponse(NULL);
        delete r;
        this->closeConnection(client);
      }
	  else if (this->_clients[i].getResponse() 
	  && this->_clients[i].getResponse()->getCGI() != NULL
	  && this->_clients[i].getResponse()->getCGI()->responseReady()){
			this->_clients[i].getResponse()->prepareCGIResponse();
	  
    }
  }
  //check servers if done
}
}

Response *Listener::sendRequestToServer(Request &req) {
  std::map<std::string, std::string> headers = req.headers();
  const std::string host = headers["Host"];
  if (host.empty())
    throw std::runtime_error("400 Bad Request");
  for (size_t i = 0; i < this->_servers.size(); i++) {
    if (this->_servers[i].serverName() == host) {
      return this->_servers[i].generateResponse(req);
    }
  }
  return this->_servers[DEFAULT_SERVER].generateResponse(req);
}

void Listener::respond(Client &client) {
  try {
    Request req = Request(client.getRequestBuffer());
    Response *r = this->sendRequestToServer(req);
    log("===== RESPONSE ====");
    r->print();
    log("===================");
    client.setResponse(r);
    // Could check if client wants to keep connection
    // if (req.getHeaders()["Connection"])
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    Response* badRequest = new Response(400);
    client.setResponse(badRequest);
  }
}

void Listener::closeConnection(Client &client) {
  std::vector<Client>::iterator it = this->_clients.begin();
  std::vector<struct pollfd>::iterator itPoll = this->_pollFds.begin();
  std::vector<Client>::iterator ite = this->_clients.end();

  for (; it != ite; it++) {
    if ((*it).getSocketfd() == client.getSocketfd()) {
      close(client.getSocketfd());
      this->_clients.erase(it);
      this->_pollFds.erase(itPoll);
      return;
    }
    itPoll++;
  }
}

bool Listener::checkValid() {
	std::map<std::string, uint32_t> map;
	for (size_t i = 0; i < this->_servers.size(); i++) {
		if (map.find(_servers[i].serverName()) == map.end())
			map[_servers[i].serverName()] = 420;
		else
			throw std::runtime_error("duplicate server_name");
	}
	return true;
}
