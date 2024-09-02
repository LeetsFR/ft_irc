#include "libft.hpp"

string printTime() {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 80, "[%X] ", timeinfo);
  return string(buffer);
}
int convertIntSafe(string n) {
  char *endPtr;
  long safe = strtol(n.c_str(), &endPtr, 10);
  if (*endPtr != '\0' && *endPtr != '\n')
    throw logic_error("Error: invalid argument");
  if (errno == ERANGE)
    throw logic_error("Error: out of range argument");
  return safe;
}
