#include "Server.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stack>

#include "Request.hpp"
#include "utils.hpp"
#include "Location.hpp"
#include "defaults.hpp"
#include "logging.hpp"
#include "Response.hpp"

#define _BACKLOG 5

Server::~Server() {}
Server::Server() {}

Server::Server(std::string &serverString): _host(DEFAULT_HOST), _listen(0), 
	_clientMaxBodySize(DEFAULT_MAX_CLIENT_BODY_SIZE), _serverName("") {
	std::istringstream iss(serverString);
		
	std::string line, s1, s2;
	while (std::getline(iss, line)) {
		Location::removeTrailing(line);
		if (find(line.begin(), line.end(), '{') != line.end()){
			std::istringstream iss2(line);
			std::getline(iss2, s1, ' ');
			if (s1 != "location")
				std::runtime_error("Unidentified object in server body");
			std::getline(iss2, s2, ' ');
			std::getline(iss, line, '}');
			Location temp(line, s2);
			_locations[temp.getUri()] = temp;
			continue ;
		}
		std::istringstream iss2(line);
		std::getline(iss2, s1, ' ');
		std::getline(iss2, s2, ';' ) ;
		if (s1 == "host"){
			this->_host = s2; 
		}
		if (s1 == "listen"){
			uint16_t	temp;
			std::istringstream iss3(s2);
			iss3 >> temp;
			if (iss3.fail())
				std::runtime_error("Incorrect parameter for listen.");
			this->_listen = temp;  
		}
		if (s1 == "server_name"){
			this->_serverName = s2; 
		}
    if (s1 ==  "client_max_body_size"){
			uint16_t	temp;
			std::istringstream iss3(s2);
			iss3 >> temp;
			if (iss3.fail())
				std::runtime_error("Incorrect parameter for Client Max Body Size.");
			this->_clientMaxBodySize = temp;  
		}
    if (s1 == "error_page"){
			std::stack<std::string> store;
      std::istringstream iss3(s2);
      while (std::getline(iss3,s2, ' '))
        store.push(s2);
      std::string key = store.top();
      store.pop();
      while (store.size() > 0){
        _errorPages[key] = store.top();
        store.pop();
      }
       
		}
	}
}

void Server::print() const {
  std::cout << std::endl;
  std::cout << "======= " << this->_serverName << " =======" << std::endl;
  std::cout << std::endl;
  std::cout << "Host: " << this->_host << std::endl;
  std::cout << "Port: " << this->_listen << std::endl;
  std::cout << "Max Body Size: " << _clientMaxBodySize << std::endl;
  std::cout << "Error Page: " << std::endl;
  for (std::map<std::string, std::string>::const_iterator i = _errorPages.begin();
           i != _errorPages.end(); ++i)
    std::cout << (*i).first << ":" << (*i).second << std::endl;
  
  std::cout << "Locations: ";

  std::map< std::string, Location>::const_iterator it =
      this->_locations.begin();
  std::map< std::string, Location>::const_iterator ite =
      this->_locations.end();

  for (; it != ite; it++) {
    std::cout << (*it).first << std::endl;
    (*it).second.print();
    std::cout <<  std::endl;

  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "=======================" << std::endl;
  std::cout << std::endl;
}

const uint16_t& Server::port() const {
  return this->_listen;
}

const std::string& Server::serverName() const {
  return this->_serverName;
}

Response* Server::generateResponse(Request& req) {
  std::string uri = req.getUri();
  std::string location = uri.substr(0, uri.find("/", 1) - 1);
  std::string resource = this->_locations[location].getRoot() + uri;
  log("resource: " << resource);
  std::ifstream index(resource);
  if (index.is_open()) {
    std::string line;
    std::string all;
    while (std::getline(index, line))
      all += line;
    index.close();
    Response* response = new Response();
    response->setBody(all);
    response->setResponseStatusCode(200);
    response->initHeaders();
    response->generateResponseData();
    std::cout << this->_serverName << " is generating a response.." << std::endl;
    return response;
  }
  else {
    Response* response = new Response(404);
    return response;
  }
  return NULL;
}
