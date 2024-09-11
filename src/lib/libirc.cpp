#include "libirc.hpp"
#include "IRC.hpp"
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
  int pos = message.find('#'); // a modifier les channel ne sont pas que des #
  int pos2 = message.find(' ', pos);
  int pos3 = message.find(' ', pos2 + 1);

  channelName = message.substr(pos, pos2 - pos);
  kickUserName = message.substr(pos2 + 1, pos3 - pos2 - 1);
  reason = message.substr(pos3 + 2);
  return true;
}

bool joinParsing(string &message, vector<string> &channel, vector<string> password) {
  int typeEndPos = message.find(' ');
  int channelEndPos = message.find(' ', typeEndPos);
  string channelBuffer = message.substr(typeEndPos, channelEndPos - typeEndPos);
  int start = typeEndPos;
  int end;
  while ((end = channelBuffer.find(',', start)) != string::npos) {
    start = end;
    channel.push_back(message.substr(start, end));
  }
  if (channelEndPos != string::npos) {
    string passwordBuffer = message.substr(channelEndPos);
    start = channelEndPos;
    while ((end = channelBuffer.find(',', start)) != string::npos) {
      start = end;
      password.push_back(message.substr(start, end));
    }
    return true;
  }

  bool getMessage(int fd, string &message) {
    char buffer[RECV_SIZE];
    int read_size = RECV_SIZE;

    while (read_size == RECV_SIZE) {
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
