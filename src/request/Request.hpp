#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>

class Request {
 public:
  Request();
  ~Request();

  void parse(const std::string &);
  void getRequestLine(const std::string &);
  void print() const;

 private:
  void parseUri(const std::string &);
  std::string _method;
  std::string _uri;
  std::string _httpVersion;
  std::string _queryString;
};

#endif  // __REQUEST_HPP__
