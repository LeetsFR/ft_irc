#include "libft.hpp"
#include <cerrno>
#include <stdexcept>

int convertIntSafe(string n) {
  char *endPtr;
  long safe = strtol(n.c_str(), &endPtr, 10);
  if (*endPtr != '\0' && *endPtr != '\n')
    throw logic_error("Error: invalid argument");
  if (errno == ERANGE)
    throw logic_error("Error: out of range argument");
  return safe;
}
