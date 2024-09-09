#include "Event.hpp"
#include "Client.hpp"
#include "Channel.hpp"

Event::Event(string &message, Client &client, typeMsg type, IRC &serv) : _serv(serv) {

  switch (type) {
  case PING:
    break;
  case PRIVMSG:
    break;
  case KICK:
    break;
  }
}

void Event::_managePING(Client &client) {
  std::string pongMessage = REP_PONG(client.getNickname());
  if (send(client.getSocket(), pongMessage.c_str(), pongMessage.size(), 0) == -1)
    cerr << RED "Error: fail to send message" RESET << endl;
}

void Event::_manageKICK(string &message, Client &client) {
  vector<string> content = kickParsing(message);
  string channelName, kickUser, reason;
  channelName = content.begin();
  kickUser = channelName + 1;
  reason = channelName + 2;
  Channel channel = _serv.findChannel(channelName);
}
