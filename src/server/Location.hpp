#ifndef __LOCATION_HPP__
#define __LOCATION_HPP__

#include <cstddef>
#include <string>
#include <vector>

#include "redirection.hpp"

class Location {
public:
  Location();
  ~Location();
  Location(std::string &text, std::string &uri);

  /* Getters */
  const std::string &getUri() const;
  const std::string &getRoot() const;
  const std::string &getAlias() const;
  const std::string &getIndex() const;
  const std::string &getReturn() const;
  const std::string &getSaveFile() const;
  const std::vector<std::string> &getAllowedMethods() const;
  const size_t &getMaxClientBodySize() const;
  const Redirection &getRedirect() const;
  const bool &getAutoIndex() const;

  /* Setters */
  void setUri(const std::string &);
  void setRoot(const std::string &);
  void setIndex(const std::string &);
  void setAllowedMethods(const std::vector<std::string> &);
  void setMaxClientBodySize(const size_t &);
  void setRedirect(const Redirection &);
  // void                              setAutoIndex(const bool&);
  void print() const;
  static void removeTrailing(std::string &);

  void parseRedirection();

private:
  std::string _uri;
  std::string _root;
  std::string _index;
  std::vector<std::string> _allowedMethods;
  size_t _maxClientBodySize;
  Redirection _redirect;
  std::string _alias;
  std::string _saveFile;
  bool _autoIndex;
  std::string _return;
  void _setPriv(std::string line);
};

#endif // __LOCATION_HPP__
