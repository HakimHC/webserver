#ifndef __REDIRECTION_HPP__
#define __REDIRECTION_HPP__

#include <string>

typedef struct {
  int statusCode;
  std::string redirLocation;
} Redirection;

enum redirectionType { DIRECTORY_REDIRECT = 301, NORMAL_REDIRECT = 0 };

#endif // __REDIRECTION_HPP__
