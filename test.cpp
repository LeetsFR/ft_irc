#include <iostream>

using namespace std;

bool inviteParsing(string &message, string &clientName, string &channelName) {
  int endInvite = message.find(' ');
  int endClientName = message.find(' ', endInvite + 1);
  clientName = message.substr(endInvite + 1, endClientName - endInvite - 1);
  channelName = message.substr(endClientName + 1);
  return true;
}

int main(int argc, char *argv[]) {
  string message = argv[1];
  string clientName, channelName;
  inviteParsing(message, clientName, channelName);
  cout << clientName << endl;
  cout << channelName << endl;
}
