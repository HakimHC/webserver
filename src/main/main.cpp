#include <iostream>

#include "Client.hpp"
#include "Location.hpp"
#include "HTTP.hpp"
#include "Server.hpp"

int main() {
  HTTP webServer = HTTP("webserv.conf");

  /* webServer.print(); */
  webServer.start();
}
