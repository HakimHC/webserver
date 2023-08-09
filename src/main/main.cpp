#include <iostream>

#include "Client.hpp"
#include "Location.hpp"
#include "HTTP.hpp"
#include "Server.hpp"

int main() {
  HTTP http;

  Server s1;
  Server s2;

  s1.initialize(8080);
  s2.initialize(8181);
  http.addServer(s1);
  http.addServer(s2);

  http.start();
  return 0;
}
