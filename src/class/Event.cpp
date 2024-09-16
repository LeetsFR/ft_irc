#include "Event.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "IRC.hpp"
#include "libirc.hpp"
#include <csignal>

Event::Event(string &message, Client &client, typeMsg type, IRC &serv) : _serv(serv) {

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
    _manageKICK(message, client); // a refaire
    break;
  case INVITE:
    _manageINVITE(message, client);
    break;
  case TOPIC:
    _manageTOPIC(message, client);
    break;
  case MODE_I:
    _manageMode_I(message, client);
    break;
  case MODE_T:
    _manageMode_T(message, client);
    break;
  case MODE_K:
    _manageMode_K(message, client);
    break;
  case MODE_O:
    _manageMode_O(message, client);
    break;
  case MODE_L:
    _manageMode_L(message, client);
    break;
  }
}
void Event::_managePING(Client &client) {
  string pongMessage = REP_PONG(client.getNickname());
  if (send(client.getSocket(), pongMessage.c_str(), pongMessage.size(), 0) == -1)
    cerr << RED "Error: fail to send message" RESET << endl;
}

void Event::_manageKICK(string &message, Client &client) {
  string channelName, kickUserName, reason;
  kickParsing(message, channelName, kickUserName, reason);
  Channel *channel = _serv.findChannel(channelName);
  if (channel->clientIsOperator(client) == false) {
    cerr << printTime() << RED "Error: client is not operator he can't KICK" RESET << endl;
    return;
  }
  Client &kickUser = _serv.findClient(kickUserName);
  channel->kickClient(kickUser);
  channel->sendAllClient(message);
}

void Event::_manageTOPIC(string &message, Client &client) {
  string channelName, topic, msg;
  bool isTopicIsChanged = topicParsing(message, channelName, topic);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    msg = ERR_NOSUCHCHANNEL(client.getHostname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    msg = ERR_NOTONCHANNEL(client.getHostname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (isTopicIsChanged == true) {
    if (channel->getProtectedTopic() == true) {
      if (channel->clientIsOperator(client) == false)
        msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName());
      sendRC(msg, client.getSocket());
      return;
    }
    channel->modifyTopic(topic);
    msg = RPL_TOPIC(client.getHostname(), channel->getName(), channel->getTopic());
    channel->sendAllClient(msg);
    return;
  } else {
    if (channel->getTopic() == "") {
      msg = RPL_NOTOPIC(client.getHostname(), channel->getName());

    } else {
      msg = RPL_TOPIC(client.getHostname(), channel->getName(), channel->getTopic());
    }
    sendRC(msg, client.getSocket());
    return;
  }
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
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(clientName, channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  const Client *invitedClient = channel->findClient(clientName);
  if (invitedClient) {
    string msg = ERR_USERONCHANNEL(invitedClient->getHostname(), invitedClient->getNickname(),
                                   channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->getInviteOnly() == true) {
    if (channel->clientIsOperator(client) == false) {
      string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channelName);
      sendRC(msg, client.getSocket());
      return;
    }
    channel->addInvitedClient(clientName);
  }
  string msg = ":" + client.getHostname() + " 341 " + invitedClient->getNickname() + " " +
               channel->getName() + "\r\n";
  sendRC(msg, invitedClient->getSocket());
  return;
}

void Event::_manageMode_I(string &message, Client &client) {
  string channelName, mode;
  modeParsing(message, channelName, mode);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (mode[0] == '+')
    channel->setInviteOnly(true);
  else if (mode[0] == '-')
    channel->setInviteOnly(false);
}

void Event::_manageMode_T(string &message, Client &client) {
  string channelName, mode;
  modeParsing(message, channelName, mode);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (mode[0] == '+')
    channel->setTopicOnlyOperator(true);
  else if (mode[0] == '-')
    channel->setTopicOnlyOperator(false);
}

void Event::_manageMode_K(string &message, Client &client) {
  string channelName, mode, param;
  modeParamParsing(message, channelName, mode, param);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (mode[0] == '+')
    channel->setPassword(param);
  else if (mode[0] == '-') {
    string tmp = "";
    channel->setPassword(tmp);
  }
}

void Event::_manageMode_O(string &message, Client &client) {
  string channelName, mode, param;
  modeParamParsing(message, channelName, mode, param);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName());
    sendRC(msg, client.getSocket());
    return;
  }
  if (mode[0] == '+')
    channel->setPassword(param);
  else if (mode[0] == '-') {
    string tmp = "";
    channel->setPassword(tmp);
  }
}
