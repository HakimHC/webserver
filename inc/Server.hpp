#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <cstdint>
# include <string>
# include <vector>

# define _NUM_ALLOWED_METHODS 3

class Server {

  public:
    Server();
    ~Server();

    void  print() const;

    typedef enum { GET, POST, DELETE } METHODS;

  private:
    uint16_t                      _port;
    int                           _socketFd;
    std::string                   _name;
    std::string                   _root;
    std::string                   _index;
    std::vector<Server::METHODS>  _allowedMethods;

    // Config                     _config;

};

#endif // __SERVER_HPP__
