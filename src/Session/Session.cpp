#include "Session.hpp"

Session::Session() {
	_sessionId = generateSessionId();
}

Session::~Session() {}

//function to create a random id for each session

std::string Session::generateSessionId() {

	std::stringstream ss;
	ss << time(NULL);
	return ss.str();
}
