#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <string>

class Client {
public:
  Client(int);
  ~Client();

  void setSocketFd(const int &);
  void setRequestBuffer(const std::string &);
  const int &getSocketfd() const;
  const std::string &getRequestBuffer() const;

private:
  int _fd;
  std::string _requestBuffer;
};

#endif // __CLIENT_HPP__
