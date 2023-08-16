#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>

class Request {
 public:
  Request();
  ~Request();

  void parseLegacy(const std::string &);

  void print() const;

  const std::map<std::string, std::string>& headers() const;

 private:
  void parseUri(const std::string &);
  std::string                           _method;
  std::string                           _uri;
  std::string                           _httpVersion;
  std::string                           _queryString;
  std::string                           _body;
  std::map<std::string, std::string>    _headers;

  std::string getRequestLineLegacy(const std::string &);
  std::string getRequestHeadersLegacy(const std::string &);
};

#endif  // __REQUEST_HPP__
