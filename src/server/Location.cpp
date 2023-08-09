#include "Location.hpp"

Location::~Location() {}
Location::Location() {
  this->_index = DEFAULT_INDEX;
  this->_root = DEFAULT_ROOT;
}

const std::string& Location::getUri() const {
  return this->_uri;
}
const std::string& Location::getRoot() const {
  return this->_root;
}
const std::string& Location::getIndex() const {
  return this->_index;
}
const std::vector<std::string>& Location::getAllowedMethods() const {
  return this->_allowedMethods;
}
const size_t& Location::getMaxClientBodySize() const {
  return this->_maxClientBodySize;
}
const std::string& Location::getDefaultFileDirectory() const {
  return this->_defaultFileDirectory;
}
const std::string& Location::getRedirect() const {
  return this->_redirect;
}
const bool& Location::getListing() const {
  return this->_listing;
}
void Location::setUri(const std::string& s) {
  this->_uri = s;
}
void Location::setRoot(const std::string& s) {
  this->_root = s;
}
void Location::setIndex(const std::string& s) {
  this->_index = s;
}
void Location::setAllowedMethods(const std::vector<std::string>& s) {
  this->_allowedMethods = s;
}
void Location::setMaxClientBodySize(const size_t& s) {
  this->_maxClientBodySize = s;
}
void Location::setDefaultFileDirectory(const std::string& s) {
  this->_defaultFileDirectory = s;
}
void Location::setRedirect(const std::string& s) {
  this->_redirect = s;
}
void Location::setListing(const bool& s) {
  this->_listing = s;
}
