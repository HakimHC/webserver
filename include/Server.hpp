#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <poll.h>

#include <cstdint>
#include <string>
#include <vector>

#include "Client.hpp"

#define _NUM_ALLOWED_METHODS 3
#define _MAX_BUFFER_SIZE 2048

class Server {
public:
  Server();
  ~Server();

  void print() const;
  void initialize(uint16_t);
  void operate();

  typedef std::string HTTPMethods;

private:
  void initPoll();
  void acceptClient();
  void readClientData(const size_t &);

  uint16_t _port;
  int _socketFd;
  std::string _name;
  std::string _root;
  std::string _index;
  std::vector<Server::HTTPMethods> _allowedMethods;
  size_t _maxClientBodySize;
  std::string _defaultFileDirectory;

  /* Parallel vectors for the clients and their respective pollfds. */
  std::vector<Client> _clients;
  std::vector<struct pollfd> _pollFds;

  // ErrorPages                 _defaultErrorPages;
  // Redirection*               _redirections;
};

#endif // __SERVER_HPP__
