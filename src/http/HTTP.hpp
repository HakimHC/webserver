#ifndef __HTTP_HPP__
#define __HTTP_HPP__

#include <vector>

#include "Listener.hpp"
#include "Server.hpp"

class HTTP {
public:
  HTTP();
  HTTP(std::string confFile);
  ~HTTP();

  void addServer(Server);
  void print() const;
  void start();

private:
  /* std::vector<Server> _servers; */
  std::vector<Listener *> _listeners;
};

#endif // __HTTP_HPP__
