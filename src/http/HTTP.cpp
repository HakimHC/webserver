#include "HTTP.hpp"
#include "Listener.hpp"
#include "Location.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <unistd.h>

HTTP::HTTP() {}
HTTP::~HTTP() {
  for (size_t i = 0; i < this->_listeners.size(); i++)
    delete this->_listeners[i];
}

void HTTP::addServer(Server server) {
  /* server.print(); */
  for (size_t i = 0; i < this->_listeners.size(); i++) {
    if (this->_listeners[i]->port() == server.port()) {
      this->_listeners[i]->addServer(server);
      return ;
    }
  }
  Listener *listener = new Listener(server.port());
  listener->addServer(server);
  this->_listeners.push_back(listener);

}

HTTP::HTTP(std::string file) {
  std::ifstream inputFile(file);

  if (!inputFile.is_open()) {
    throw(std::runtime_error("Error could not open file."));
  }
  std::string line;
  while (std::getline(inputFile, line)) {
    Location::removeTrailing(line);
	if (line.size() == 0)
		continue ;
    std::istringstream iss(line);
    std::string s1, s2;
    std::getline(iss, s1, ' ');
    std::getline(iss, s2, '#');
    if (s1.size() == 0 && s2.size() == 0)
		continue ;
	if (s1 != "server" && std::find(s2.begin(), s2.end(),'{') != s2.end())
      throw (std::runtime_error("Block is not a server"));
    long count = -1;
    std::string serverString = "";
    while (count != 0) {
      std::getline(inputFile, s1, '}');
      count -= std::count(s1.begin(), s1.end(), '{');
      count++;
      serverString += s1 + "}";
    }
    addServer(Server(serverString));
  }
}

void HTTP::start() {
  while (true) {
    for (size_t i = 0; i < this->_listeners.size(); i++) {
      this->_listeners[i]->_listen();
      usleep(100);
    }
  }
}

void HTTP::print() const {
  for (unsigned int i = 0; i < _listeners.size(); i++) {
    _listeners[i]->print();
  }
}
