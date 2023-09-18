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
}

void CGI::startCGI(){
	char	**env;
	env = new char*[2];
	env[0] = new char[_queryString.size() + 1];
	std::strcpy(env[0],_queryString.c_str());
	env[1] = NULL;
	if (pipe(_pip) == -1){
		_failed = true;
		return ;
	}
	_id = fork();
	if (_id == -1){
		delete[] env[0];
		delete[] env;
		_failed = true;
		return ;
	}
	if (_id == 0) { 
		close(_pip[0]);
		if (dup2(_pip[1], STDOUT_FILENO) == -1)
			log("whaaat");
		close(_pip[1]);
		int w = execle(DEFAULT_PYTHON_ROUTE, DEFAULT_PYTHON_ROUTE, _resourcePath.c_str(), NULL, env);
		log("Fatal execle " << w);
		exit(EXIT_FAILURE);
	}
	close(_pip[1]);
	fcntl(_pip[0], F_SETFL, O_NONBLOCK);
	_pfd.fd = _pip[0];
	_pfd.events = POLLIN;
	delete[] env[0];
	delete[] env;
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


// Response *CGI::returnPythonCGI(Request &req) {
// 	std::cout << "" << " is generating a response.." << std::endl;
// 	errno = 0;
// 	char buffer[1024];
// 	bzero(buffer, sizeof(buffer));
// 	getcwd(buffer, sizeof(buffer) - 1);
// 	std::string resourcePath(buffer);
// 	resourcePath += "/" +req.getResource();
// 	std::string all = executepythonCGI(resourcePath,req.getQueryString());
//     if (all.length() > 0){
// 		Response *response = new Response();
// 		std::vector<std::string> separeted = separatePyCGI(all);
// 		response->setExtension(".py");
// 		response->setBody(separeted[2]);
// 		response->addHeader(separeted[0], separeted[1]);
// 		response->setResponseStatusCode(200);
// 		response->initHeaders();
// 		response->generateResponseData();
// 		return response;
// 	} else {
//     	log("CGI  failed [" << req.getResource() << "] (" << strerror(errno) << ")");
//    		Response *response;
//     	if (errno == ENOENT)
//       		response = new Response(404);
//     	else
//       		response = new Response(403);
//     	return response;
//   }
// }


std::string	CGI::executepythonCGI(std::string script, std::string queryString)
{
	int		id;
	int		pip[2];
	int		status;
	char	**env;
	std::string	result;
	char	buffer[10];
	//bool	error = false;

	bzero(buffer, 10 * sizeof(char));
	queryString = "QUERY_STRING="+ queryString;
	env = new char*[2];
	env[0] = new char[queryString.size() + 1];
	std::strcpy(env[0],queryString.c_str());
	env[1] = NULL;
	if (pipe(pip) == -1)
		return "";
	id = fork();
	if (id == -1){
		delete[] env[0];
		delete[] env;
		return "";
	}
	if (id == 0) { 
		close(pip[0]);
		if (dup2(pip[1], STDOUT_FILENO) == -1)
			log("whaaat");
		close(pip[1]);
		int w = execle(DEFAULT_PYTHON_ROUTE, DEFAULT_PYTHON_ROUTE, script.c_str(), NULL, env);
		log("Fatal execle " << w);
		exit(EXIT_FAILURE);
	}
	close(pip[1]);
	while (read(pip[0], buffer, 9) > 0) {
		result += std::string(buffer);
		bzero(buffer, sizeof(buffer));
	}
	waitpid(id,&status, 0);
	close(pip[0]);
	delete[] env[0];
	delete[] env;
	return result;
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