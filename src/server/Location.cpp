#include <algorithm>
#include <iostream>
#include <sstream>

#include "Location.hpp"
#include "defaults.hpp"

Location::~Location() {}
Location::Location() : _root(DEFAULT_ROOT), _index(DEFAULT_INDEX) {}

const std::string &Location::getUri() const { return this->_uri; }
const std::string &Location::getRoot() const { return this->_root; }
const std::string &Location::getAlias() const { return this->_alias; }
const bool &Location::getAutoIndex() const { return this->_autoIndex; }
const std::string &Location::getIndex() const { return this->_index; }
const std::string &Location::getReturn() const { return this->_return; }
const std::string &Location::getSaveFile() const { return this->_saveFile; }
const std::vector<std::string> &Location::getAllowedMethods() const {
  return this->_allowedMethods;
}
const size_t &Location::getMaxClientBodySize() const {
  return this->_maxClientBodySize;
}
const Redirection &Location::getRedirect() const { return this->_redirect; }
void Location::setUri(const std::string &s) { this->_uri = s; }
// void Location::setAutoIndex(const bool& s) {
//   this->_autoIndex = s;
// }
void Location::setRoot(const std::string &s) { this->_root = s; }
void Location::setIndex(const std::string &s) { this->_index = s; }
void Location::setAllowedMethods(const std::vector<std::string> &s) {
  this->_allowedMethods = s;
}
void Location::setMaxClientBodySize(const size_t &s) {
  this->_maxClientBodySize = s;
}
void Location::setRedirect(const Redirection &s) { this->_redirect = s; }
bool Location::notSpace(char c) { return (!std::isspace(static_cast<unsigned char>(c))); }

void Location::removeTrailing(std::string &str) {
  std::string::iterator it1;
  std::string::reverse_iterator it2;
  it1 = find_if(str.begin(), str.end(), notSpace);
  it2 = find_if(str.rbegin(), str.rend(), notSpace);
  if (it1 == str.end() || it2 == str.rend())
    str.clear();
  else
    str.assign(it1, it2.base());
}

Location::Location(std::string &text, std::string &uri)
    : _uri(uri), _index(DEFAULT_INDEX),
      _maxClientBodySize(DEFAULT_MAX_CLIENT_BODY_SIZE), _alias(""),
      _saveFile(""), _autoIndex(false), _return("") {
  std::istringstream iss(text);
  std::string line, s1, s2;
	memset(&this->_redirect, 0, sizeof(_redirect));
  while (std::getline(iss, line)) {
    removeTrailing(line);
    _setPriv(line);
  }
  if (_alias == "" && _root == "")
	_root = DEFAULT_ROOT;
  if (this->_allowedMethods.size() == 0)
    this->_allowedMethods.push_back("GET");
	if (!checkValid()){
		throw std::runtime_error("Location has incompatible params.");
	}
}

void Location::_setPriv(std::string line) {
  std::string s1, s2, st1, leftover;
  std::istringstream iss2(line);
  std::getline(iss2, s1, ' ');
  std::getline(iss2, s2, ';');
  std::istringstream iss3(s2);
  if (s1 == "max_body_size") {
    size_t temp;
    iss3 >> temp;
    if (iss3.fail())
      throw std::runtime_error("Incorrect parameter for Max Client Body Size.");
	std::getline(iss3,leftover);
	if (leftover.size() >0 && std::find_if(leftover.begin(), leftover.end(),
		notSpace) != leftover.end())
      throw std::runtime_error("Incorrect parameter for Max Client Body Size.");
    this->_maxClientBodySize = temp;
  }
  else if (s1 == "allowed_methods" || s1 == "allow") {
    while (std::getline(iss3, st1, ' ')) {
      if (!st1.empty())
        _allowedMethods.push_back(st1);
    }
  }
  else if (s1 == "root") {
    std::getline(iss3, st1);
    _root = st1;
  }
  else if (s1 == "index") {
    std::getline(iss3, st1);
    _index = st1;
  }
  else if (s1 == "alias") {
    std::getline(iss3, st1);
    _alias = st1;
  }
  else if (s1 == "autoindex") {
    std::getline(iss3, st1);
    _autoIndex = (st1 == "on");
  }
  else if (s1 == "save_file") {
    std::getline(iss3, st1);
    _saveFile = st1;
  }
  else if (s1 == "return") {
    std::getline(iss3, st1);
    _return = st1;
    this->parseRedirection();
  }
  else if (s1 == "" || s1[0] == '#')
	return ;
  else
	throw (std::runtime_error("Unknown directive error. (" + s1 + ")"));

}

void Location::print() const {
  std::cout << "uri:" << _uri << std::endl;
  std::cout << "root:" << _root << std::endl;
  std::cout << "index:" << _index << std::endl;
  std::cout << "maxClientBodySize:" << _maxClientBodySize << std::endl;
  std::cout << "redirect.statusCode:" << _redirect.statusCode << std::endl;
  std::cout << "redirect.location:" << _redirect.redirLocation << std::endl;
  std::cout << "alias:" << _alias << std::endl;
  std::cout << "saveFile:" << _saveFile << std::endl;
  std::cout << "autoIndex:" << _autoIndex << std::endl;
  std::cout << "return:" << _return << std::endl;
  for (unsigned int i = 0; i < _allowedMethods.size(); i++) {
    std::cout << "allowed method:" << _allowedMethods[i] << std::endl;
  }
}

void Location::parseRedirection() {
  if (this->_return.empty())
    return;
  std::string token;
  std::stringstream tokenizer(this->_return);
  std::cout << "keloke papi" << std::endl;

  // First token
  std::getline(tokenizer, token, ' ');
  this->_redirect.statusCode = std::atoi(token.c_str());
  // Second token
  // token
  std::getline(tokenizer, token, ' ');
  this->_redirect.redirLocation = token;
}

bool Location::checkValid(){
	if (_redirect.statusCode != 0 && (_redirect.statusCode <300 
		|| _redirect.statusCode >309))
		throw std::runtime_error("Invalid redirection Code.");
	if (_root.size() > 0 && _alias.size() > 0)
		throw std::runtime_error("A location cannot have both root and alias.");
	if (_uri.size() == 0)
		throw std::runtime_error("Uri cannot be empty");
	return (1);
}
