#include <iostream>

#include "Client.hpp"
#include "Location.hpp"
#include "HTTP.hpp"
#include "Server.hpp"

int main() {
  HTTP http("config.cnf2");
  http.print();
  return 0;
}
