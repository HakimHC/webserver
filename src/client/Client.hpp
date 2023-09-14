#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <string>

#include "Response.hpp"

class Client {
public:
  Client(int);
  ~Client();

  void setSocketFd(const int &);
  void setRequestBuffer(const std::string &);
  const int &getSocketfd() const;
  const std::string &getRequestBuffer() const;

  const Response* getResponse() const;
  Response* getResponse();
  void  setResponse(Response*);

private:
  int _fd;
  std::string _requestBuffer;

  Response* _response;

};

#endif // __CLIENT_HPP__
