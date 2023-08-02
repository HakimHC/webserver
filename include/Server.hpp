#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <cstdint>
# include <string>
# include <vector>

# include <poll.h>

# include "Client.hpp"

# define _NUM_ALLOWED_METHODS   3
# define _MAX_BUFFER_SIZE       2048

class Server {

  public:
    Server();
    Server(uint16_t port);
    ~Server();

    void  print() const;
    void  operate();
    void  acceptClient();
    void  readClientData(const size_t&);

    typedef enum { GET, POST, DELETE } METHODS;

  private:
    uint16_t                      _port;
    int                           _socketFd;
    std::string                   _name;
    std::string                   _root;
    std::string                   _index;
    std::vector<Server::METHODS>  _allowedMethods;
    size_t                        _maxClientBodySize;
    std::string                   _defaultFileDirectory;

    std::vector<Client>           _clients;
    std::vector<struct pollfd>    _pollFds;

    // Polling stuff (pollfd vectors)
    // ErrorPages                 _defaultErrorPages;
    // Redirection*               _redirections;

};

#endif // __SERVER_HPP__
