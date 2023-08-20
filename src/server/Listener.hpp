#ifndef __LISTENER_HPP__
#define __LISTENER_HPP__

#include <string>
#include <vector>

#include <poll.h>

#include "Client.hpp"
#include "Request.hpp"
#include "Server.hpp"

class Listener {
public:
  Listener(uint16_t port);
  ~Listener();

  void addServer(Server &);
  const uint16_t &port() const;
  void print() const;
  void _listen();
  Response *sendRequestToServer(Request &);
  void respond(Client &);

private:
  uint16_t _port;
  int _socketFd;
  std::vector<Client> _clients;
  std::vector<struct pollfd> _pollFds;
  std::vector<Server> _servers;

  void initPoll();
  void acceptClient();
  void readClientData(const size_t &);
  void closeConnection(Client &);
};

#endif // __LISTENER_HPP__
