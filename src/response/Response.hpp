#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <map>

class Response {
 public:
   Response();
   ~Response();
 private:
   void initStatusCodes();
   std::string                        _statusLine;
   std::map<std::string, std::string> _headers;
   std::map<int, std::string>         _statusCodes;
};

#endif // __RESPONSE_HPP__
