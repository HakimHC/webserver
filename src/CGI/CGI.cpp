#include "CGI.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>

#include <iostream>
#include <sstream>
#include <cstring>

#include "defaults.hpp"
#include "logging.hpp"
#include "utils.hpp"

CGI::CGI(Request &req): _collecting(true),_failed(false), _result(""){
	std::cout << "" << " is generating a response.." << std::endl;
	errno = 0;
	char buffer[1024];
	bzero(buffer, sizeof(buffer));
	getcwd(buffer, sizeof(buffer) - 1);
	std::string resourcePath(buffer);
	_resourcePath = resourcePath + "/" +req.getResource();
	_queryString = "QUERY_STRING="+ req.getQueryString();
	_requestMethod = "REQUEST_METHOD=" + req.getMethod();
	_requestURI = "REQUEST_URI=" + req.getUri();
	setHeadersEnvVar( req.getHeaders());

}

void CGI::startCGI(){
	setEnv();
	if (pipe(_pip) == -1){
		_failed = true;
		return ;
	}
	_id = fork();
	if (_id == -1){
		freeEnv();
		_failed = true;
		return ;
	}
	if (_id == 0) { 
		close(_pip[0]);
		if (dup2(_pip[1], STDOUT_FILENO) == -1)
			log("whaaat");
		close(_pip[1]);
		log("Launching CGI");
		int w = execle(_resourcePath.c_str(), _resourcePath.c_str(),  NULL, _env);
		log("Fatal execle " << w);
		exit(EXIT_FAILURE);
	}
	close(_pip[1]);
	fcntl(_pip[0], F_SETFL, O_NONBLOCK);
	_pfd.fd = _pip[0];
	_pfd.events = POLLIN;
	freeEnv();
}


bool CGI::responseReady(){
	char	buffer[1 << 12];
	int		status, ret;
	
	log("Checking if response is ready");
	bzero(buffer, 1 << 12 * sizeof(char));
	if ((ret = poll(&_pfd, 1, 500)) > 0) {
        if (_pfd.revents & POLLIN) {
            ssize_t n = read(_pip[0], buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0'; 
                _result += std::string(buffer);
                bzero(buffer, sizeof(buffer));
            } else {
                _collecting = false;
            }
        }
    }
	if (_collecting == false){
		waitpid(_id,&status, 0);
		close(_pip[0]);
		return true;
	}
	return false;
}


Response *CGI::prepareResponse(){
	Response *response = new Response();
	response->setCGI(this); 
	response->startTimer(); 
	return response;
}


 std::vector<std::string> CGI::separatePyCGI(std::string all){
	std::istringstream iss(all);
	std::vector<std::string> toReturn;
	std::string hold, hold2;

	std::getline(iss, hold, ' ');
	toReturn.push_back(hold);
	std::getline(iss, hold);
	toReturn.push_back(hold);
	std::getline(iss, hold);
	while (std::getline(iss, hold))
		hold2  += hold + "\n";
	toReturn.push_back(hold2);
	
	return toReturn;
  }

std::string CGI::getResult(){
	return this->_result;
}

std::string CGI::getResourcePath(){
	return this->_resourcePath;
}

void CGI::setHeadersEnvVar(const std::map<std::string, std::string> &headers){
	std::map<std::string, std::string>::const_iterator itHead = headers.begin();
	while (itHead != headers.end()){
		std::string ini = "HTTP_" + itHead->first;
		for (std::string::iterator it = ini.begin(); it != ini.end(); it++){
			if (*it == '-')
				*it = '_';
			else
				*it = std::toupper(static_cast<unsigned char>(*it));
		}
		_httpHeadersEnv[ini] = itHead->second;
		itHead++;
	}
}

void CGI::setEnv(){
		
	_env = new char*[4 + _httpHeadersEnv.size()];
	(_env)[0] = new char[_queryString.size() + 1];
	std::strcpy((_env)[0],_queryString.c_str());
	(_env)[1] = new char[_requestMethod.size() + 1];
	std::strcpy((_env)[1],_requestMethod.c_str());
	(_env)[2] = new char[_requestURI.size() + 1];
	std::strcpy((_env)[2],_requestURI.c_str());
	int i =3;
	for (std::map<std::string, std::string>::iterator it = _httpHeadersEnv.begin(); 
				it != _httpHeadersEnv.end(); it++){
		(_env)[i] = new char[(*it).first.size() +(*it).second.size() + 2];
		std::strcpy((_env)[i],((*it).first + "=" + (*it).second).c_str());
		i++;
	}
	(_env)[3 + _httpHeadersEnv.size()] = NULL;
	_envSize = i;
}

void CGI::freeEnv(){
	for(unsigned int i = 0; i < _envSize; i++){
		delete[] _env[i];
	}
	delete[] _env;
}