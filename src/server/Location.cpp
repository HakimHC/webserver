#include "Location.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

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

bool NotSpace(char c){
	return (!std::isspace(static_cast<unsigned char>(c)));
}

void Location::removeTrailing (std::string &str){
	std::string::iterator it1;
	std::string::reverse_iterator it2;
	it1 = find_if(str.begin(),str.end(),NotSpace);
	it2 = find_if(str.rbegin(),str.rend(),NotSpace);
	if (it1 == str.end() || it2 ==str.rend())
		str.clear();
	else
		str.assign(it1, it2.base());
}


Location::Location(std::string &text, std::string &uri): _uri(uri), _root(""),
	_index(""), _maxClientBodySize(0), _defaultFileDirectory(""), _redirect(""),
	_alias(""), _saveFile(""), _autoIndex(0), _return(""), _listing(0){
	std::istringstream iss(text);
	std::string line, s1, s2;
	while (std::getline(iss, line)){
		removeTrailing(line);
		_setPriv(line);
	}
}

void Location::_setPriv(std::string line){
	std::string s1, s2, st1;
	std::istringstream iss2(line);
	std::getline(iss2, s1, ' ');
	std::getline(iss2, s2, ';');
	std::istringstream iss3(s2);
	if (s1 == "max_body_size"){
		size_t temp;
		iss3 >> temp;
		if (iss3.fail())
			std::runtime_error("Incorrect parameter for Max Client Body Size.");
		this->_maxClientBodySize = temp; 
	}
	if (s1 == "allowed_methods" || s1 == "allow"){
		while (std::getline(iss3, st1))
			_allowedMethods.push_back(st1);
	}
	if (s1 == "redirect"){
		std::getline(iss3, st1);
		_redirect = st1;
	}
	if (s1 == "listing"){
		bool temp;
		iss3 >> temp;
		if (iss3.fail())
			std::runtime_error("Incorrect parameter for listing.");
		this->_listing = temp; 
	}
	if (s1 == "root"){
		std::getline(iss3, st1);
		_root = st1;
	}
	if (s1 == "index"){
		std::getline(iss3, st1);
		_index = st1;
	}
	if (s1 == "alias"){
		std::getline(iss3, st1);
		_alias = st1;
	}
	if (s1 == "autoindex"){
		std::getline(iss3, st1);
		_autoIndex = (st1 == "on");
	}
	if (s1 == "save_file"){
		std::getline(iss3, st1);
		_saveFile = st1;
	}
	if (s1 == "return"){
		std::getline(iss3, st1);
		_return = st1;
	}
}


void Location::print() const{
	std::cout << "uri:" << _uri << std::endl;
	std::cout << "root:" << _root << std::endl;
	std::cout << "index:" << _index << std::endl;
	std::cout << "maxClientBodySize:" << _maxClientBodySize << std::endl;
	std::cout << "defaultFileDirectory:" << _defaultFileDirectory << std::endl;
	std::cout << "redirect:" << _redirect << std::endl;
	std::cout << "alias:" << _alias << std::endl;
	std::cout << "saveFile:" << _saveFile << std::endl;
	std::cout << "autoIndex:" << _autoIndex << std::endl;
	std::cout << "return:" << _return << std::endl;
	std::cout << "listing:" << _listing << std::endl;
	for (unsigned int i = 0; i < _allowedMethods.size(); i++){
		std::cout << "allowed method:" << _allowedMethods[i] << std::endl;
	}
}