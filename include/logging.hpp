#ifndef __LOGGING_HPP__
#define __LOGGING_HPP__

#include <iostream>
#include "utils.hpp"

#define log(message) std::cout << message << std::endl
#define LOG_INFO(message) std::cout << "[ " << mGetTime() << " ]: " << message << std::endl

#endif // __LOGGING_HPP__
