#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

class Client {
 public:
  Client(int);
  ~Client();

  void setSocketFd(const int&);
  const int& getSocketfd() const;

 private:
  int _fd;
};

#endif  // __CLIENT_HPP__
