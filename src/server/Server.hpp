#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <poll.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define _NUM_ALLOWED_METHODS 3
#define _MAX_BUFFER_SIZE 2048

class Server {
public:
  Server();
  ~Server();
  Server(std::string &serverString);

  void print() const;
  const uint16_t &port() const;
  const std::string &serverName() const;
  Response *generateResponse(Request &);

  typedef std::string HTTPMethods;

  static bool isDirectory(std::string const &);

  Response *handleGetRequest(Request &);
  Response *handlePostRequest(Request &);
  Response *handleDeleteRequest(Request &);

  Response *returnIndexFile(const std::string &);
  Response *returnPythonCGI(Request &req);
  bool isMethodAllowed(const Request &);

private:
  std::string _host;
  uint16_t _listen;
  uint16_t _clientMaxBodySize;
  std::string _serverName;
  std::map<std::string, Location> _locations;     // key = _uri
  std::map<std::string, std::string> _errorPages; // Meter valores por defecto
    
  std::vector<std::string> *readDirectoryContent(const std::string &) const;
  void acceptClient();
  void readClientData(const size_t &);
  Response *generateAutoIndex(const std::string &s);
  void setResponseErrorPages(const Request &);
  std::string concatWithRootOrAlias(const Request &);
  Response *returnRedirection(const Request &, int);
  bool locationExists(const Request &) const;
  bool checkValid() const;
  bool isPythonCGIReq( Request &req ) ;
  std::vector<std::string> separatePyCGI(std::string );
  std::string executepythonCGI(std::string script, std::string queryString);
};

#endif // __SERVER_HPP__
