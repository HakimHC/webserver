#include <iostream>

#include "Server.hpp"

int main() {
  Server* server = new Server(8080);
  Server* server2 = new Server(8181);

  server->print();
  server2->print();
  delete server;
  delete server2;
}
