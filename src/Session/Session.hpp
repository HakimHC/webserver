#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <ctime>
#include <sstream>

class Session
{
	private:
		std::string _sessionId;

		std::string generateSessionId();
	public:
		Session();
		~Session();
};

#endif
