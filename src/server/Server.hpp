#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <poll.h>

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "Location.hpp"
#include "Request.hpp"
#include "Response.hpp"


#define _NUM_ALLOWED_METHODS 3
#define _MAX_BUFFER_SIZE 2048

class Server {
  public:
    Server();
    ~Server();
	  Server (std::string &serverString);

    void  print() const;
    const uint16_t& port() const;
    const std::string& serverName() const;
    Response*  generateResponse(Request&);

    typedef std::string HTTPMethods;

  private:
    void  acceptClient();
    void  readClientData(const size_t&);

    std::string						              _host;
    uint16_t                            _listen;
    uint16_t                            _clientMaxBodySize;
	  
    std::string                      	  _serverName;
	  std::map< std::string, Location>	  _locations; //key = _uri
	  std::map <std::string, std::string> _errorPages; //Meter valores por defecto
};

#endif  // __SERVER_HPP__
