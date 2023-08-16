#include <iostream>

#include "Client.hpp"
#include "Location.hpp"
#include "HTTP.hpp"
#include "Server.hpp"

int main() {
  HTTP http("webserv.conf");
  http.print();
  http.start();
  return 0;
}
