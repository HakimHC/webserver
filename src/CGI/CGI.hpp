#ifndef __CGI_HPP__
#define __CGI_HPP__
#include "CGI.hpp"
#include <poll.h>
#include "Request.hpp"
#include "Response.hpp"

class CGI {
	public:
	CGI() {}
	~CGI() {}
	CGI(Request &req);
	//Response *returnPythonCGI(Request &req);
	static bool isPythonCGIReq(Request &req);
	std::string	executepythonCGI(std::string script, 
		std::string queryString);
	static std::vector<std::string> separatePyCGI(std::string all);
	void startCGI();
	bool responseReady();
	Response *prepareResponse();
	
	private:
		bool		_collecting;
		std::string _resourcePath;
		int			_id;
		int			_pip[2];
		std::string	_queryString;
		bool		_failed;
		std::string	_result;
		struct pollfd _pfd;
};



#endif