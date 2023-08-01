#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <cstdint>
# include <string>
# include <vector>

class Server {

  public:
    typedef enum { GET, POST, DELETE } METHODS;

  private:
    uint16_t                      _port;
    int                           _socketFd;
    std::string                   _name;
    std::string                   _root;
    std::string                   _index;
    std::vector<Server::METHODS>  _allowedMethods;

};

#endif // __SERVER_HPP__
