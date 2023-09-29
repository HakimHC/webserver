#ifndef __CGI_HPP__
#define __CGI_HPP__
#include <poll.h>
#include "Request.hpp"
#include "Response.hpp"

class Response;

class CGI {
	public:
	CGI() {}
	~CGI();
	CGI(Request &req);
	//Response *returnPythonCGI(Request &req);
	static bool isPythonCGIReq(Request &req);
	static std::vector<std::string> separatePyCGI(std::string all);
	static std::map<std::string, std::string> separateHeader(std::string all);
	void startCGI();
	bool responseReady();
	Response *prepareResponse();
	std::string getResult();
	std::string getResourcePath();
	void setHeadersEnvVar(const std::map<std::string, std::string> &headers);
	void setEnv();
	void freeEnv();

	private:
		bool		_collecting;
		std::string _resourcePath;
		int			_id;
		int			_pip[2];
		int			_pipOut[2];
		std::string	_queryString; //done
		std::string _requestMethod; //done
		std::string _contentType;
		std::string _contentLength;
		std::string _pathInfo;
		std::string _pathTranslated;
		std::string _scriptName;
		std::string _requestURI; //done
		std::string _remoteAddr;
		std::string _remoteHost;
		std::string _serverName;
		std::string _serverPort;
		std::string _serverProtocol;
		std::string _serverSoftware;
		std::string _unsentBody;
		std::map<std::string, std::string> _httpHeadersEnv;
		bool		_failed;
		std::string	_result;
		struct pollfd _pfd;
		struct pollfd _pfdOut;
		char 		  **_env;
		unsigned int	_envSize;
		unsigned int	_sendingBufferSize;
		bool			_writing;
		int				_exitstatus;
		
};



#endif