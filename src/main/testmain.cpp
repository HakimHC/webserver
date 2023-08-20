#include <iostream>

#include "Client.hpp"
#include "HTTP.hpp"
#include "Location.hpp"
#include "Server.hpp"

int main() {
  HTTP http("webserv.conf");
  http.print();
  http.start();
  return 0;
}
