#include <cstdlib>
#include <exception>
#include <iostream>

#include "Client.hpp"
#include "HTTP.hpp"
#include "Location.hpp"
#include "Server.hpp"
#include "defaults.hpp"
#include "logging.hpp"

int errUsage() {
  log("fatal: invalid arguments");
  log("usage: ./webserv [path_to_config]");
  return EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
  if (argc > 2)
    return errUsage();

  std::string configFile = (argv[1] ? argv[1] : DEFAULT_CONFIG_FILE);

  /* webServer.print(); */
  try {
    HTTP webServer = HTTP(configFile);
   webServer.start();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  system("leaks -q webserv");
  return EXIT_SUCCESS;
}
