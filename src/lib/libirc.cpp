#include "libirc.hpp"
#include "IRC.hpp"
#include <string>

bool portIsValid(int port) {
  if (port == 194 || (port >= 6665 && port <= 6669) || port == 6697)
    return true;
  return false;
}

void sendRC(const string &message, int fd) {
  if (send(fd, message.c_str(), message.size(), 0) == -1)
    cerr << printTime() << RED "Error: fail to send message" RESET << endl;
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

bool joinParsing(string &message, string &channel, string &password) {
  string::size_type typeEndPos = message.find(' ');
  string::size_type channelEndPos = message.find(' ', typeEndPos + 1);
  channel = message.substr(typeEndPos + 1, channelEndPos - typeEndPos - 1);
  if (channelEndPos != string::npos)
    password = message.substr(channelEndPos + 1);
  else
    password.clear();
  return true;
}

bool inviteParsing(string &message, string &clientName, string &channelName) {
  int endInvite = message.find(' ');
  int endClientName = message.find(' ', endInvite + 1);
  clientName = message.substr(endInvite + 1, endClientName - endInvite - 1);
  channelName = message.substr(endClientName + 1);
  return true;
}

bool topicParsing(string &message, string &channelName, string &topicName) {
  size_t endTopic = message.find(' ');
  size_t endChannelName = message.find(':', endTopic + 1);
  if (endChannelName == string::npos) {
    channelName = message.substr(endTopic + 1);
    return false;
  } else {
    channelName = message.substr(endTopic + 1, endChannelName - endTopic - 2);
    topicName = message.substr(endChannelName + 1);
    return true;
  }
}

bool kickParsing(const string &message, string &channelName, string &kickUserName, string &reason) {
  size_t pos = message.find(' ');
  if (pos == string::npos)
    return false;

  size_t pos2 = message.find(' ', pos + 1);
  if (pos2 == string::npos)
    return false;

  size_t pos3 = message.find(' ', pos2 + 1);
  if (pos3 == string::npos) {
    pos3 = message.length();
  }

  channelName = message.substr(pos + 1, pos2 - pos - 1);
  kickUserName = message.substr(pos2 + 1, pos3 - pos2 - 1);

  if (pos3 < message.length()) {
    size_t reasonStart = pos3 + 1;
    if (message[reasonStart] == ':') {
      reasonStart += 1;
    }
    reason = message.substr(reasonStart);
  } else {
    reason = "";
  }

  return true;
}

void privmsgParsing(const string &message, string &recipient, string &msgContent) {
  size_t pos = message.find(' ');
  string rest = message.substr(pos + 1);
  pos = rest.find(' ');
  recipient = rest.substr(0, pos);
  msgContent = rest.substr(pos + 2);
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

int getLimitSafe(const string &n) {
  char *endPtr;
  long safe = strtol(n.c_str(), &endPtr, 10);
  if (*endPtr != '\0' && *endPtr != '\n')
    return -1;
  if (errno == ERANGE)
    return -1;
  return safe;
}

bool modeParamParsing(string &message, string &channelName, string &mode, string &param) {
  int endType = message.find(' ');
  int endChannel = message.find(' ', endType + 1);
  int end = message.find(' ', endChannel + 1);
  channelName = message.substr(endType + 1, endChannel - endType - 1);
  mode = message.substr(endChannel + 1, end - endChannel - 1);
  param = message.substr(end + 1);
  return true;
}

bool modeParsing(string &message, string &channelName, string &mode) {
  int endType = message.find(' ');
  int endChannel = message.find(' ', endType + 1);
  channelName = message.substr(endType + 1, endChannel - endType - 1);
  mode = message.substr(endChannel + 1);
  return true;
}
