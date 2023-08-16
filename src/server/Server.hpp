#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <poll.h>

#include <cstdint>
#include <string>
#include <vector>
#include <map>
//#include "Client.hpp"
#include "Location.hpp"


#define _NUM_ALLOWED_METHODS 3
#define _MAX_BUFFER_SIZE 2048

class Server {
  public:
    Server();
    ~Server();
	Server (std::string &serverString);
	//Server (const std::Map<std::string, std::string> &source);

  void  print() const;
  const uint16_t& port() const;
    // void  initialize();
    // void  operate();

    typedef std::string HTTPMethods;

  private:
    void  acceptClient();
    void  readClientData(const size_t&);

    std::string						            _host;
    uint16_t                          _listen;
    uint16_t                          _clientMaxBodySize;
	  
    std::string                      	_serverName;
	  std::map< std::string, Location>	_locations; //key = _uri
	  std::map <std::string, std::string> _errorPage; //Meter valores por defecto

    /* Parallel vectors for the clients and their respective pollfds. */
    int                           _socketFd;
   // std::vector<Client>           _clients;
    std::vector<std::string>      _clientBuffer;
    std::vector<struct pollfd>    _pollFds;

    // ErrorPages                 _defaultErrorPages;
    // Redirection*               _redirections;

};

#endif  // __SERVER_HPP__
