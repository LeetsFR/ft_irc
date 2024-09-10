#include "libirc.hpp"
#include "IRC.hpp"
#include <iterator>
#include <vector>

bool portIsValid(int port) {
  if (port == 6667)
    return true;
  return false;
}

vector<string> ft_split(const string &str, const string &delim) {
  vector<string> vec;
  size_t posBegin = 0;
  size_t posEnd = 0;
  if (str.empty() || delim.empty())
    return vec;
  while (true) {
    posEnd = str.find(delim, posBegin);
    if (posEnd == string::npos)
      break;
    if (posBegin != posEnd)
      vec.push_back(str.substr(posBegin, posEnd - posBegin));
    posBegin = posEnd + delim.size();
  }
  if (posBegin < str.size())
    vec.push_back(str.substr(posBegin, string::npos));
  return (vec);
}

void handleSigint(int sig) {
  run = false;
  (void)sig;
  cout << "\r";
}

bool kickParsing(string &message, string &channelName, string &kickUserName, string &reason) {

  int pos = message.find(' ');
  int pos2 = message.find(pos + 1, ' ');

  channelName = string(message, 0, pos);
  kickUserName = string(message, pos + 1, pos2);
  reason = string(message, pos2 + 1, string::npos);
  cout << "Channel = " << channelName << endl;
  cout << "kickusername = " << kickUserName << endl;
  cout << "reason = " << reason << endl;
  return true;
}
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
  char buffer[sizeof("[00:00:00] ")];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, sizeof("[00:00:00] "), "[%X] ", timeinfo);
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
