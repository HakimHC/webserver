#ifndef __REQUEST_HPP__
# define __REQUEST_HPP__

# include <string>

class Request {
  public:
    Request();
    ~Request();

  private:
    std::string   _method;
    std::string   _uri;
    std::string   _httpVersion;
    std::string   _queryString;
};

#endif // __REQUEST_HPP__
