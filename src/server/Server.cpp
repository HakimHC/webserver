#include "Server.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>

#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "defaults.hpp"
#include "logging.hpp"
#include "utils.hpp"

#define _BACKLOG 5

Server::~Server() {}
Server::Server() {}


Server::Server(std::string &serverString)
    : _host(DEFAULT_HOST), _listen(0),
      _clientMaxBodySize(DEFAULT_MAX_CLIENT_BODY_SIZE), _serverName("") {
  std::istringstream iss(serverString);

  std::string line, s1, s2, leftover;
  while (std::getline(iss, line)) {
    Location::removeTrailing(line);
    if (line[0] == '#')
		continue ;
	if (find(line.begin(), line.end(), '{') != line.end()) {
      std::istringstream iss2(line);
      std::getline(iss2, s1, ' ');
      if (s1 != "location")
        throw std::runtime_error("Unidentified object in server body");
      std::getline(iss2, s2, ' ');
      std::getline(iss, line, '}');
      Location temp(line, s2);
      // if (_locations.find(temp.getUri()) == _locations.end())
      _locations[temp.getUri()] = temp;
      // temp.print();
      continue;
    }
    std::istringstream iss2(line);
    std::getline(iss2, s1, ' ');
    std::getline(iss2, s2, ';');
    if (s1 == "host") {
      this->_host = s2;
    }
    else if (s1 == "listen") {
      uint16_t temp;
      std::istringstream iss3(s2);
      iss3 >> temp;
      if (iss3.fail())
       	throw std::runtime_error("Incorrect parameter for listen.");
	std::getline(iss3,leftover);
	if (leftover.size() >0 && std::find_if(leftover.begin(), leftover.end(),
		Location::notSpace) != leftover.end())
      throw std::runtime_error("Incorrect parameter for listen port.");
      this->_listen = temp;
    }
    else if (s1 == "server_name") {
      this->_serverName = s2;
    }
    else if (s1 == "client_max_body_size") {
      uint16_t temp;
      std::istringstream iss3(s2);
      iss3 >> temp;
      if (iss3.fail())
        throw std::runtime_error("Incorrect parameter for Client Max Body Size.");
	std::getline(iss3,leftover);
	if (leftover.size() >0 && std::find_if(leftover.begin(), leftover.end(),
		Location::notSpace) != leftover.end())
      throw std::runtime_error("Incorrect parameter for Max Client Body Size.");
      this->_clientMaxBodySize = temp;
    }
    else if (s1 == "error_page") {
      std::stack<std::string> store;
      std::istringstream iss3(s2);
      while (std::getline(iss3, s2, ' '))
        store.push(s2);
      std::string key = store.top();
      store.pop();
      while (store.size() > 0) {
        _errorPages[key] = store.top();
        store.pop();
      }
    }
	else if (s1 == "" || s1[0] == '#' || s1[0] == '}')
	 continue ;
	else
		throw (std::runtime_error("Unknown directive error."));
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
  for (std::map<std::string, std::string>::const_iterator i =
           _errorPages.begin();
       i != _errorPages.end(); ++i)
    std::cout << (*i).first << ":" << (*i).second << std::endl;

  std::cout << "Locations: ";

  std::map<std::string, Location>::const_iterator it = this->_locations.begin();
  std::map<std::string, Location>::const_iterator ite = this->_locations.end();

  for (; it != ite; it++) {
    std::cout << (*it).first << std::endl;
    (*it).second.print();
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "=======================" << std::endl;
  std::cout << std::endl;
}

const uint16_t &Server::port() const { return this->_listen; }

const std::string &Server::serverName() const { return this->_serverName; }

bool Server::isMethodAllowed(const Request &req) {
  bool existingLocation = locationExists(req);

  if (!existingLocation && req.getMethod() != "GET")
    return false;
  else if (existingLocation) {
    std::vector<std::string> allowedMethod =
        this->_locations[req.getLocation()].getAllowedMethods();
    if (std::find(allowedMethod.begin(), allowedMethod.end(),
                  req.getMethod()) == allowedMethod.end())
      return false;
  }
  return true;
}

std::string Server::concatWithRootOrAlias(const Request &req) {
  bool existingLocation = locationExists(req);
  std::string resource;

  if (!existingLocation) {
    resource = DEFAULT_ROOT + req.getUri();
  } else if (this->_locations[req.getLocation()].getAlias().empty()) {
    resource = this->_locations[req.getLocation()].getRoot() + req.getUri();
  } else {
    std::string alias = this->_locations[req.getLocation()].getAlias();
    std::string uri = req.getUri();
    resource = uri.replace(1, req.getUri().find("/", 1) - 1, alias)
                   .substr(1, uri.size());
  }
  return resource;
}

Response* Server::returnResponse(const int& statusCode) {
  // std::stringstream ss;
  // ss << statusCode;
  // std::map<std::string, std::string>::iterator it;

  // it = this->_errorPages.find(ss.str());
  // log("First: " << it->first);
  // log("Second: " << it->second);
  // if (it == this->_errorPages.end())
  //   return new Response(statusCode);
  // const bool isDir = Server::isDirectory(req.getResource().substr(0, req.getResource().size() - 1));
  // std::string res = req.getResource();
  // if (isDir) {
  //   res += it->second;
  // }
  // else {
  //   res.replace(res.find_last_of("/") + 1, res.size(), it->second);
  // }

  // return this->returnIndexFile(res);

  std::map<std::string, std::string>::iterator it;
  std::stringstream ss;
  ss << statusCode;
  it = this->_errorPages.find(ss.str());
  if (it == this->_errorPages.end())
    return new Response(statusCode);

  log("First: " << it->first);
  log("Second: " << it->second);
  Response* r = new Response(302);
  r->addHeader("Location", it->second);
  r->generateResponseData();
  return r;
}

Response *Server::generateResponse(Request &req) {
  std::string uri = req.getUri();
  std::string location = uri.substr(0, uri.find("/", 1));
  req.setLocation(location);
  req.setResource(this->concatWithRootOrAlias(req));

  if (req.getBody().size() > this->_clientMaxBodySize)
    return returnResponse(413);
  if (req.getHttpVersion() != "HTTP/1.1")
    return returnResponse(505);
  if (!this->isMethodAllowed(req))
    return returnResponse(405);
  if (locationExists(req) &&
      !this->_locations[req.getLocation()].getReturn().empty())
    return this->returnRedirection(req, NORMAL_REDIRECT);


  Response *response;

  switch (Request::convertRequestType(req.getMethod())) {
  case GET:
    response = handleGetRequest(req);
    break;
  case POST:
    response = handlePostRequest(req);
    break;
  case DELETE:
    response = handleDeleteRequest(req);
    break;
  default:
    response = returnResponse(405);
    break;
  }
  return response;
}

Response *Server::handleGetRequest(Request &req) {
  if (Server::isDirectory(req.getResource()) &&
      req.getUri()[req.getUri().size() - 1] != '/')
    return this->returnRedirection(req, DIRECTORY_REDIRECT);
  else if (Server::isDirectory(
               req.getResource().substr(0, req.getResource().size() - 1))) {
    std::string index;
    std::string noindex = req.getResource();
    if (this->_locations.find(req.getLocation()) != this->_locations.end())
      index = this->_locations[req.getLocation()].getIndex();
    else
      index = DEFAULT_INDEX;
    req.setResource(req.getResource() + index);
    bool autoindex = false;
    if (locationExists(req))
      autoindex = this->_locations[req.getLocation()].getAutoIndex();
    if (access(req.getResource().c_str(), F_OK) == EXIT_SUCCESS)
      return this->returnIndexFile(req.getResource());
    else if (autoindex == true)
      return this->generateAutoIndex(noindex);
    else
      return returnResponse(403);
  } else
    return this->returnIndexFile(req.getResource());
}

Response *Server::returnIndexFile(const std::string &resource) {
  std::cout << this->_serverName << " is generating a response.." << std::endl;
  errno = 0;
  std::ifstream index(resource);
  if (index.is_open()) {
    std::string all;
    // char buf[DEFAULT_MAX_CLIENT_BODY_SIZE];

    // while (!index.eof()) {
    //     memset(buf, 0, sizeof(buf));
    //     index.read(buf, DEFAULT_MAX_CLIENT_BODY_SIZE);
    //     all += buf;
    // }
    std::string line;
    while (std::getline(index, line))
      all += line + "\n";
    index.close();
    Response *response = new Response();
    response->setExtension(
        resource.substr(resource.find("."), resource.size()));
    response->setBody(all);
    response->setResponseStatusCode(200);
    response->initHeaders();
    response->generateResponseData();
    return response;
  } else {
    log("open() failed [" << resource << "] (" << strerror(errno) << ")");
    Response *response;
    if (errno == ENOENT)
      response = returnResponse(404);
    else
      response = returnResponse(403);
    return response;
  }
}

bool Server::isDirectory(std::string const &path) {
  struct stat fileInfo;

  if (stat(path.c_str(), &fileInfo))
    return false;

  return S_ISDIR(fileInfo.st_mode);
}

std::vector<std::string> *
Server::readDirectoryContent(const std::string &path) const {
  std::vector<std::string> *res = new std::vector<std::string>;

  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir(path.c_str())) == NULL)
    throw std::runtime_error("error: unable to open directory");

  while ((entry = readdir(dir))) {
    std::string curr = entry->d_name;
    if (curr != "." && curr != "..") {
      res->push_back(curr);
    }
  }
  closedir(dir);
  return res;
}

Response *Server::generateAutoIndex(const std::string &s) {
  std::vector<std::string> *contents;
  try {
    contents = this->readDirectoryContent(s);
  } catch (...) {
    return returnResponse(403);
  }
  contents->insert(contents->begin(), s);
  Response *r = returnResponse(*contents);
  delete contents;
  return r;
}

void Server::setResponseErrorPages(const Request &req) {
  static_cast<void>(req);
}

bool Server::locationExists(const Request &req) const {
  return (this->_locations.find(req.getLocation()) != this->_locations.end());
}

Response *Server::returnRedirection(const Request &req, int statusCode) {
  if (statusCode == DIRECTORY_REDIRECT) {
    Response *response = returnResponse(301);
    std::stringstream ss;
    ss << this->_listen;
    response->addHeader("Location",
                        "http://localhost:" + ss.str() + req.getUri() + "/");
    response->generateResponseData();
    return response;
  }
  // this->_locations[req.getLocation()].parseRedirection();
  const Redirection &redir = this->_locations[req.getLocation()].getRedirect();
  std::string url;
  if (redir.redirLocation[0] == '/') {
    std::stringstream port;
    port << this->_listen;
    url = "http://localhost:" + port.str() + redir.redirLocation;
  } else
    url = redir.redirLocation;
  Response *response = returnResponse(redir.statusCode);
  response->addHeader("Location", url);
  response->generateResponseData();
  return response;
}

Response* Server::handlePostRequest(Request& req) {
  const Location& loc = this->_locations[req.getLocation()];
  std::string uploadDirectory = (loc.getAlias().empty() ? loc.getRoot() : loc.getAlias()) + "/" + loc.getSaveFile();
  log("Save file directory: " << uploadDirectory);
  log("Resource: " << req.getResource());
  std::string resource = req.getResource();
  std::string outfileName = resource.substr(resource.find_last_of("/") + 1, resource.size());
  log("Final file name: " << outfileName);
  std::ofstream uploadedFile(uploadDirectory + "/" + outfileName);
  if (!uploadedFile.is_open())
    return returnResponse(403);
  uploadedFile << req.getBody();
  uploadedFile.close();
  return new Response(201);
}

Response* Server::handleDeleteRequest(Request& req) {
  std::string resource = req.getResource();
  if (Server::isDirectory(resource)) return returnResponse(403);
  log("Resource: " << resource);
  errno = 0;
  if (remove(resource.c_str()) != 0 && errno != ENOENT) return returnResponse(500);
  else if (errno == ENOENT) return returnResponse(404);
  return new Response(204);
}

bool Server::checkValid() const{
std::map<std::string, std::string>::const_iterator it1 = _errorPages.begin();
while(it1 != _errorPages.end()){
	std::string leftover;
	std::istringstream iss(it1->second);
	int u;
	iss >> u;
	if (iss.fail() || !(u >= 300 && u < 600))
      throw std::runtime_error("Incorrect parameter error pages.");
	std::getline(iss,leftover);
	if (leftover.size() >0 && std::find_if(leftover.begin(), leftover.end(),
		Location::notSpace) != leftover.end())
      throw std::runtime_error("Incorrect parameter for error pages.");

}

return (1);
}
