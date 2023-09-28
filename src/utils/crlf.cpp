#include "utils.hpp"

bool utils::isCrLf(std::string const &s) { return (s == "\r\n"); }

std::string mGetTime() {
  time_t currentTime;

  struct tm *timeInfo;
  char timeBuffer[80];

  time(&currentTime);
  timeInfo = gmtime(&currentTime);

  strftime(timeBuffer, sizeof(timeBuffer), "%a, %d %b %Y %H:%M:%S GMT",
           timeInfo);
  return timeBuffer;
}
