#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <string>
# include <map>
# include <vector>

# define HTTP_VERSION "HTTP/1.1"

class Response {
 public:
   Response();
   Response(int);
   Response(const std::vector< std::string >&); // For generating autoindexes;
   ~Response();

   const std::string& getData() const;
   void setBody(const std::string&);

   void addHeader(const std::string& key, const std::string& value);
   void initHeaders();
   void generateResponseData();
   void setResponseStatusCode(const int&);

   void setErrorPageFile(const int&, const std::string&);

   void setExtension(const std::string&);

 private:
   void initStatusCodes();
   void initMimeTypes();
   int                                _responseStatusCode;
   std::map<int, std::string>         _statusCodesMap;
   std::map<std::string, std::string>         _mimeTypesMap;
   std::map<std::string, std::string> _headers;
   std::string                        _body;

   std::string                        _allData;
   std::map<int, std::string>         _errorPageFiles;

   std::string                        _extension;

   void generateCurrentDateTime();
};

#endif // __RESPONSE_HPP__
