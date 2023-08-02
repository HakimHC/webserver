#include <iostream>

#include "Server.hpp"
#include "Client.hpp"

int main() {
  Server *s1 = NULL;
  try {
    s1 = new Server(8181);
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  s1->print();
  s1->operate();
  delete s1;
}
