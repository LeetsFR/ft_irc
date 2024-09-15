#include "Event.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "IRC.hpp"
#include "libirc.hpp"

Event::Event(string &message, Client &client, typeMsg type, IRC &serv)
    : _serv(serv) {

  switch (type) {
  case ERROR:
    break;
  case PRIVMSG:
    break;
  case PING:
    _managePING(client);
    break;
  case JOIN:
    _manageJOIN(message, client);
    break;
  case KICK:
    _manageKICK(message, client);
    break;
  case INVITE:
    _manageINVITE(message, client);
    break;
  case TOPIC:
    break;
  case MODE_I:
    break;
  case MODE_T:
    break;
  case MODE_K:
    break;
  case MODE_O:
    break;
  case MODE_L:
    break;
  }
}
void Event::_managePING(Client &client) {
  string pongMessage = REP_PONG(client.getNickname());
  if (send(client.getSocket(), pongMessage.c_str(), pongMessage.size(), 0) ==
      -1)
    cerr << RED "Error: fail to send message" RESET << endl;
}

void Event::_manageKICK(string &message, Client &client) {
  string channelName, kickUserName, reason;
  kickParsing(message, channelName, kickUserName, reason);
  Channel *channel = _serv.findChannel(channelName);
  if (channel->clientIsOperator(client) == false) {
    cerr << printTime()
         << RED "Error: client is not operator he can't KICK" RESET << endl;
    return;
  }
  Client &kickUser = _serv.findClient(kickUserName);
  channel->kickClient(kickUser);
  channel->sendAllClient(message);
}

void Event::_manageJOIN(string &message, Client &client) {

  vector<string> channelName;
  vector<string> password;
  joinParsing(message, channelName, password);
  vector<string>::iterator it = channelName.begin();
  vector<string>::iterator jt = password.begin();
  while (it != channelName.end()) {
    Channel *channelPtr = _serv.findChannel(*it);
    if (channelPtr == NULL) {
      _serv.createChannel(*it, *jt, client);
    } else {
      channelPtr->joinChannel(*jt, client);
    }
    ++it;
    ++jt;
  }
}

void Event::_manageINVITE(string &message, Client &client) {
  string clientName, channelName;
  inviteParsing(message, clientName, channelName);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(clientName, channelName);
    sendRC(msg, client.getSocket());
  }
  Client * client = channel->findClient(clientName);
}
