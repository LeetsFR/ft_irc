#include "libirc.hpp"
#include "IRC.hpp"
#include <string>

bool portIsValid(int port) {
  if (port == 6667)
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

bool kickParsing(string &message, string &channelName, string &kickUserName, string &reason) {
  int pos = message.find('#'); // a modifier les channel ne sont pas que des #
  int pos2 = message.find(' ', pos);
  int pos3 = message.find(' ', pos2 + 1);

  channelName = message.substr(pos, pos2 - pos);
  kickUserName = message.substr(pos2 + 1, pos3 - pos2 - 1);
  reason = message.substr(pos3 + 2);
  return true;
}

<<<<<<< HEAD
bool joinParsing(string &message, vector<string> &channel, vector<string> &password) {
  string::size_type typeEndPos = message.find(' ');
  string::size_type channelEndPos = message.find(' ', typeEndPos + 1);
  string channelBuffer = message.substr(typeEndPos + 1, channelEndPos - typeEndPos - 1);
  string::size_type start = 0;
  string::size_type end;
  while ((end = channelBuffer.find(',', start)) != string::npos) {
    channel.push_back(channelBuffer.substr(start, end - start));
    start = end + 1;
  }
  channel.push_back(channelBuffer.substr(start));
  start = 0;
  if (channelEndPos != string::npos) {
    string passwordBuffer = message.substr(channelEndPos + 1);
    cout << passwordBuffer << endl;
    start = 0;
    while ((end = passwordBuffer.find(',', start)) != string::npos) {
      password.push_back(passwordBuffer.substr(start, end - start));
      start = end + 1;
    }
    password.push_back(passwordBuffer.substr(start));
  }
  while (password.size() < channel.size()) {
    password.push_back("");
  }

  vector<string>::iterator it;
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
  int endTopic = message.find(' ');
  int endChannelName = message.find(':', endTopic + 1);
  if (endChannelName == string::npos) {
    channelName = message.substr(endTopic + 1);
    return false;
  } else {
    channelName = message.substr(endTopic + 1, endChannelName - endTopic - 1);
    topicName = message.substr(endChannelName + 1);
    return true;
  }
}

void privmsgParsing(const string &message, string &recipient, string &msgContent) {
  size_t pos = message.find(' ');
  string rest = message.substr(pos + 1);

  pos = rest.find(' ');
  recipient = rest.substr(0, pos);
  msgContent = rest.substr(pos + 1);
}
=======
>>>>>>> origin/scely

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
