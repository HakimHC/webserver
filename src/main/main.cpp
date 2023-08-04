#include <iostream>

#include "Client.hpp"
#include "Server.hpp"

int main() {
  Server* s1 = NULL;
  try {
    s1 = new Server();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  s1->initialize();
  s1->print();
  s1->operate();
  delete s1;
}
