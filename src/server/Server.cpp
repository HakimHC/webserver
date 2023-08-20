#include "Server.hpp"

#include <fcntl.h>
#include <dirent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <algorithm>
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
    // if (_locations.find(temp.getUri()) == _locations.end())
    _locations[temp.getUri()] = temp;
    temp.print();
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

bool Server::isMethodAllowed(const Request& req) {
bool existingLocation = locationExists(req);

log("LOCATION: " << req.getLocation());
log("EXISTS: " << (existingLocation ? "YES" : "NO"));

if (!existingLocation && req.getMethod() != "GET") return false;
else if (existingLocation) {
  log("entra aqui en puto serio");
  std::vector<std::string> allowedMethod = this->_locations[req.getLocation()].getAllowedMethods();
  if (std::find(allowedMethod.begin(), allowedMethod.end(), req.getMethod()) == allowedMethod.end()) return false;
}
return true;
}

std::string Server::concatWithRootOrAlias(const Request& req) {
bool existingLocation = locationExists(req);
std::string resource;

// if (existingLocation) log("EXISTING"); else log("NON EXISTS");

if (!existingLocation) {
  resource = DEFAULT_ROOT + req.getUri();
}
else if (this->_locations[req.getLocation()].getAlias().empty()) {
  resource = this->_locations[req.getLocation()].getRoot() + req.getUri();
}
else {
  std::string alias = this->_locations[req.getLocation()].getAlias();
  std::string uri = req.getUri();
  resource = uri.replace(1, req.getUri().find("/", 1) - 1, alias).substr(1, uri.size());
  // log("AFTER ALIAS: " << resource);
}
return resource;
}

Response* Server::generateResponse(Request& req) {
  if (req.getBody().size() > this->_clientMaxBodySize)
    return new Response(413);
  std::string uri = req.getUri();
  std::string location = uri.substr(0, uri.find("/", 1));
  log("location: " << location);
  req.setLocation(location);
  // _locations[req.getLocation()].print();
  req.setResource(this->concatWithRootOrAlias(req));

  if (!this->isMethodAllowed(req))
    return new Response(405);
  if (locationExists(req) && !this->_locations[req.getLocation()].getReturn().empty())
    return this->returnRedirection(req, NORMAL_REDIRECT);

  Response *response;

  switch (Request::convertRequestType(req.getMethod()))
  {
  case GET:
    response = handleGetRequest(req);
    break;
  case POST:
    response = new Response(501);
    break;
  case DELETE:
    response = new Response(501);
    break;
  default:
    // response = new Response(405);
    response = handleGetRequest(req);
    break;
  }
  return response;
}

Response* Server::handleGetRequest(Request& req) {
  if (Server::isDirectory(req.getResource()) && req.getUri()[req.getUri().size() - 1] != '/')
    return this->returnRedirection(req, DIRECTORY_REDIRECT);
  else if (Server::isDirectory(req.getResource().substr(0, req.getResource().size() - 1))) {
    std::string index;
    std::string noindex = req.getResource();
    if (this->_locations.find(req.getLocation()) != this->_locations.end()) index = this->_locations[req.getLocation()].getIndex();
    else index = DEFAULT_INDEX;
    req.setResource(req.getResource() + index);
    bool autoindex = false;
    std::cout << "This location exists: " << locationExists(req) << std::endl;
    if (locationExists(req))
      autoindex = this->_locations[req.getLocation()].getAutoIndex();
    log("autoindex: " << autoindex);
    if (access(req.getResource().c_str(), F_OK ) == EXIT_SUCCESS)
      return this->returnIndexFile(req.getResource());
    // else if (autoindex == true) return new Response(501);
    else if (autoindex == true) return this->generateAutoIndex(noindex);
    else return new Response(403);
  }
  else return this->returnIndexFile(req.getResource());
}

Response* Server::returnIndexFile(const std::string& resource) {
  std::cout << this->_serverName << " is generating a response.." << std::endl;
  errno = 0;
  std::ifstream index(resource);
  if (index.is_open()) {
    std::string line;
    std::string all;
    while (std::getline(index, line))
      all += line;
    index.close();
    Response *response = new Response();
    response->setBody(all);
    response->setResponseStatusCode(200);
    response->initHeaders();
    response->generateResponseData();
    return response;
  }
  else {
    log("open() failed [" << resource << "] (" << strerror(errno) << ")");
    Response* response;
    if (errno == ENOENT)
      response = new Response(404);
    else
      response = new Response(403);
    return response;
  }
}

bool Server::isDirectory(std::string const& path) {
  struct stat fileInfo;

  if (stat(path.c_str(), &fileInfo)) return false;

  return S_ISDIR(fileInfo.st_mode);
}

std::vector<std::string>* Server::readDirectoryContent(const std::string& path) const {
  std::vector<std::string>* res = new std::vector<std::string>;

  DIR* dir;
  struct dirent* entry;

  if ((dir = opendir(path.c_str())) == NULL) throw std::runtime_error("error: unable to open directory");

  while ((entry = readdir(dir))) {
    std::string curr = entry->d_name;
    if (curr != "." && curr != "..") {
      res->push_back(curr);
    }
  }
  closedir(dir);
  return res;
}

Response* Server::generateAutoIndex(const std::string& s) {
  std::vector<std::string>* contents;
  log("trying to generate autoindex stuff");
  try {
    contents = this->readDirectoryContent(s);
  } catch(...) {return new Response(403);}
  contents->insert(contents->begin(), s);
  Response* r = new Response(*contents);
  delete contents;
  return r;
}

void Server::setResponseErrorPages(const Request& req) {
 static_cast<void>(req);
}

bool Server::locationExists(const Request& req) const {
  return (this->_locations.find(req.getLocation()) != this->_locations.end());
}

Response* Server::returnRedirection(const Request& req, int statusCode) {
  if (statusCode == DIRECTORY_REDIRECT) {
    Response* response = new Response(301);
    std::stringstream ss;
    ss << this->_listen;
    response->addHeader("Location", "http://localhost:" + ss.str() + req.getUri() + "/");
    response->generateResponseData();
    return response;
  }
  // this->_locations[req.getLocation()].parseRedirection();
  const Redirection& redir = this->_locations[req.getLocation()].getRedirect();
  std::string url;
  log("REdir location sdadsa: " << redir.redirLocation);
  if (redir.redirLocation[0] == '/') {
    std::stringstream port;
    port << this->_listen;
    url = "http://localhost:" + port.str() + redir.redirLocation;
  } else url = redir.redirLocation;
  Response* response = new Response(redir.statusCode);
  response->addHeader("Location", url);
  response->generateResponseData();
  return response;
}
