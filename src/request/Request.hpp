#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <string>
#include <map>

class Request {
 public:
  Request();
  Request(const std::string&);
  ~Request();


  void print() const;

  const std::map<std::string, std::string>& headers() const;

  const std::string&                           getMethod() const;
  const std::string&                           getUri() const;
  const std::string&                           getHttpVersion() const;
  const std::string&                           getQueryString() const;
  const std::string&                           getBody() const;
  const std::map<std::string, std::string>&    getHeaders() const;

  void                           setMethod(const std::string&);
  void                           setUri(const std::string&);
  void                           setHttpVersion(const std::string&);
  void                           setQueryString(const std::string&);
  void                           setBody(const std::string&);
  void                           setHeaders(const std::map<std::string, std::string>&);

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
  void        getRequestBodyLegacy(const std::string&);
  void parseLegacy(const std::string &);
  void printHeaders() const;
};

#endif  // __REQUEST_HPP__
