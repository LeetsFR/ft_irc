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
    _managePRIVMSG(message, client);
    break;
  case PING:
    _managePING(client);
    break;
  case JOIN:
    _manageJOIN(message, client);
    break;
  case KICK:
    _manageKICK(message, client); // a refaire parsing
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
  default:
    break;
  }
}

void Event::_managePRIVMSG(string &message, Client &client) {
  string recipient;
  string msgContent;

  privmsgParsing(message, recipient, msgContent);

  if (recipient[0] == '#') {
    Channel *channel = _serv.findChannel(recipient);
    if (channel == NULL) {
      string errorMsg = ERR_NOSUCHCHANNEL(client.getNickname(), recipient);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }

    if (channel->findClient(client.getSocket()) == false) {
      string errorMsg = ERR_CANNOTSENDTOCHAN(client.getNickname(), recipient);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }

    channel->sendAllClient(":" + client.getNickname() + " PRIVMSG " + recipient + " :" + msgContent + "\r\n");
  } else {
    Client *targetClient = _serv.findClient(recipient);
    if (targetClient == NULL) {
      string errorMsg = ERR_NOSUCHNICK(client.getNickname(), recipient);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }
    string fullMsg = ":" + client.getNickname() + " PRIVMSG " + recipient + " :" + msgContent + "\r\n";
    send(targetClient->getSocket(), fullMsg.c_str(), fullMsg.size(), 0);
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
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  Client *kickUser = _serv.findClient(kickUserName);
  if (kickUser == NULL) {
    string msg = ERR_NOSUCHNICK(client.getNickname(), kickUserName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(kickUser->getSocket()) == false) {
    string msg = ERR_USERNOTINCHANNEL(kickUserName, channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  channel->kickClient(*kickUser);
  string kickMsg = ":" + client.getNickname() + " KICK " + channelName + " " + kickUserName + " :" + reason + "\r\n";
  channel->sendAllClient(kickMsg);
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
  if (invitedClient == NULL) {
    string msg = ERR_USERONCHANNEL(invitedClient->getHostname(), invitedClient->getNickname(), channel->getName());
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
  string msg = ":" + client.getHostname() + " 341 " + invitedClient->getNickname() + " " + channel->getName() + "\r\n";
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
