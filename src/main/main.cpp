#include <iostream>

#include "Server.hpp"
#include "Client.hpp"

int main() {
  Server* server = new Server(8080);
  Server* server2 = new Server(8181);

  server->print();
  server2->print();
  delete server;
  delete server2;

  Client* client = new Client(5);
  std::cout << client->getSocketfd() << std::endl;
  delete client;
}
