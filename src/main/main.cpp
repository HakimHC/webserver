#include <iostream>

#include "Server.hpp"

int main() {
  Server* server = new Server();

  server->print();
  delete server;
}
