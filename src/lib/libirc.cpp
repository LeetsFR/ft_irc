#include "libirc.hpp"

bool quit = false;

bool portIsValid(int port) {
  if (port == 6667)
    return true;
  return false;
}

void handleSigint(int sig) { quit = sig; }

bool getMessage(int fd, string &message) {
  char buffer[RECV_SIZE];
  int read_size = RECV_SIZE;

  while (read_size == RECV_SIZE && message.find('\n') == string::npos) {
    read_size = recv(fd, buffer, RECV_SIZE, 0);
    if (read_size == 0)
      return false;
    else if (read_size == -1)
      throw logic_error("Error: Failed to recv the client socket");
    message.append(buffer, read_size);
  }
  return true;
}

string printTime() {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 80, "[%X] ", timeinfo);
  return string(buffer);
}

int convertIntSafe(const string &n) {
  char *endPtr;
  long safe = strtol(n.c_str(), &endPtr, 10);
  if (*endPtr != '\0' && *endPtr != '\n')
    throw logic_error("Error: invalid argument");
  if (errno == ERANGE)
    throw logic_error("Error: out of range argument");
  return safe;
}
