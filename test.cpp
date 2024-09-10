#include <iostream>

using namespace std;

bool kickParsing(string &message, string &channelName, string &kickUserName, string &reason) {
  int pos = message.find('#');
  int pos2 = message.find(' ', pos);
  int pos3 = message.find(' ', pos2 + 1);

  channelName = message.substr(pos, pos2 - pos);
  kickUserName = string(message, pos2 + 1, pos3 - pos2 - 1);
  reason = string(message, pos, string::npos);
  cout << "Channel = " << channelName << "X" << endl;
  cout << "kickusername = " << kickUserName << "X" << endl;
  cout << "reason = " << reason << "X" << endl;
  return true;
}

int main(int argc, char *argv[]) {
  string message(argv[1]);
  string channelName, kickUserName, reason;
  kickParsing(message, channelName, kickUserName, reason);
  return 0;
}
