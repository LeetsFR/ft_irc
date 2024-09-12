#include "Event.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"

Event::Event(string &message, Client &client, typeMsg type, IRC &serv) : _serv(serv) {

  switch (type) {
  case PING:
    _managePING(client);
    break;
  case JOIN:
    _createChannel(message);
    break;
  case PRIVMSG:
    break;
  case KICK:
    _manageKICK(message, client);
    break;
  default:
    break;
  }
}

void Event::_createChannel(string &message) { 

    (void)message;
}

void Event::_managePING(Client &client) {
  std::string pongMessage = REP_PONG(client.getNickname());
  if (send(client.getSocket(), pongMessage.c_str(), pongMessage.size(), 0) == -1)
    cerr << RED "Error: fail to send message" RESET << endl;
}

void Event::_manageKICK(string &message, Client &client) {
  string channelName, kickUserName, reason;
  kickParsing(message, channelName, kickUserName, reason);
  Channel &channel = _serv.findChannel(channelName);
  if (channel.clientIsOperator(client) == false) {
    cerr << printTime() << RED "Error: client is not operator he can't KICK" RESET << endl;
    return;
  }
  Client &kickUser = _serv.findClient(kickUserName);
  channel.kickClient(kickUser);
  channel.sendMessage(message);
}
