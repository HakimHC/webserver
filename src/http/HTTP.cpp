#include "HTTP.hpp"
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "Location.hpp"
#include <algorithm>
#include <iostream>

HTTP::HTTP(){};
HTTP::~HTTP(){};

void HTTP::addServer(Server server) { this->_servers.push_back(server); }

HTTP::HTTP(std::string file){
  std::ifstream inputFile(file);
    
    if (!inputFile.is_open()){
      throw (std::runtime_error("Error could not open file.\n"));
    }
    std::string line;
    while (std::getline(inputFile,line)){
      std::istringstream iss(line);
      Location::removeTrailing(line);
      std::string s1, s2;
      std::getline(iss, s1, ' '); 
      std::getline(iss, s2, '#');
      if (s1 != "server")
        continue ;
      long count = -1;
      std::string serverString = "";
      while(count != 0){
        std::getline(inputFile, s1, '}');
        count -= std::count(s1.begin(), s1.end(), '{');
        count ++;
        serverString += s1 + "}";
      }
      addServer(Server(serverString));
    }


}


// void HTTP::start() {
//   while (true) {
//     for (size_t i = 0; i < this->_servers.size(); i++) {
//       this->_servers[i].operate();
//       usleep(100);
//     }
//   }
// }


void HTTP::print() const{
  for (unsigned int i = 0; i < _servers.size(); i++){
    std::cout << "HTTP: Servers\n";
    _servers[i].print();
  }
}