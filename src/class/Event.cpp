#include "Event.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "IRC.hpp"
#include "libirc.hpp"
#include <csignal>
#include <unistd.h>

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

Event::~Event()
{
}

void Event::_managePRIVMSG(string &message, Client &client) {
  string channelName;
  string msgContent;

  privmsgParsing(message, channelName, msgContent);

  if (channelName[0] == '#' || channelName[0] == '&') {
    Channel *channel = _serv.findChannel(channelName);
    if (channel == NULL) {
      string errorMsg = ERR_NOSUCHCHANNEL(client.getNickname(), channelName);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }

    if (channel->findClient(client.getSocket()) == false) {
      string errorMsg = ERR_CANNOTSENDTOCHAN(client.getNickname(), channelName);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }

    channel->sendAllClient(":" + client.getNickname() + " PRIVMSG " + channelName + " :" + msgContent + "\r\n");
  } else {
    Client *targetClient = _serv.findClient(channelName);
    if (targetClient == NULL) {
      string errorMsg = ERR_NOSUCHNICK(client.getNickname(), channelName);
      send(client.getSocket(), errorMsg.c_str(), errorMsg.size(), 0);
      return;
    }
    string fullMsg = ":" + client.getNickname() + " PRIVMSG " + channelName + " :" + msgContent + "\r\n";
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
    string msg = ERR_USERNOTINCHANNEL(kickUser->getHostname(), kickUserName, channelName);
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
  cout << "\n***********Manage TOPIC***********\n";
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
    string msg = ERR_NOSUCHCHANNEL(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (!channel->findClient(client.getSocket())) {
    string msg = ERR_NOTONCHANNEL(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  Client *invitedClient = _serv.findClient(clientName);
  if (invitedClient == NULL) {
    string msg = ERR_NOSUCHNICK(client.getNickname(), clientName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (channel->findClient(invitedClient->getSocket())) {
    string msg = ERR_USERONCHANNEL(client.getNickname(), clientName, channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (channel->getInviteOnly()) {
    if (!channel->clientIsOperator(client)) {
      string msg = ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName);
      sendRC(msg, client.getSocket());
      return;
    }
    channel->addInvitedClient(invitedClient->getNickname());
  }

  string msg = RPL_INVITING(client.getNickname(), invitedClient->getNickname(), channelName);
  sendRC(msg, client.getSocket());

  string inviteMsg = ":" + client.getNickname() + " INVITE " + invitedClient->getNickname() + " :" + channelName + "\r\n";
  sendRC(inviteMsg, invitedClient->getSocket());
}

void Event::_manageMode_I(string &message, Client &client) {
  string channelName, mode;
  modeParsing(message, channelName, mode);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channelName);
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
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channelName);
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
    string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channelName);
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

void Event::_manageMode_O(std::string &message, Client &client) {
  std::string channelName, mode, targetNick;
  modeParamParsing(message, channelName, mode, targetNick);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    std::string msg = ERR_NOSUCHCHANNEL(client.getHostname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (channel->findClient(client.getSocket()) == false) {
    std::string msg = ERR_NOTONCHANNEL(client.getHostname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (channel->clientIsOperator(client) == false) {
    std::string msg = ERR_CHANOPRIVSNEEDED(client.getHostname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  Client *targetClient = _serv.findClient(targetNick);
  if (targetClient == NULL) {
    std::string msg = ERR_NOSUCHNICK(client.getNickname(), targetNick);
    sendRC(msg, client.getSocket());
    return;
  }

  if (channel->findClient(targetClient->getSocket()) == false) {
    std::string msg = ERR_USERNOTINCHANNEL(client.getNickname(), targetNick, channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (mode[0] == '+') {
    if (channel->clientIsOperator(*targetClient)) {
      std::string msg = ERR_USERONCHANNEL(client.getNickname(), targetNick, channelName);
      sendRC(msg, client.getSocket());
      return;
    }
    channel->addOperator(*targetClient);
    std::string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " +o " + targetNick + "\r\n";
    channel->sendAllClient(modeMsg);

  } else if (mode[0] == '-') {
    if (channel->clientIsOperator(*targetClient) == false) {
      std::string msg = ERR_USERNOTINCHANNEL(client.getNickname(), targetNick, channelName);
      sendRC(msg, client.getSocket());
      return;
    }
    channel->removeOperator(*targetClient);
    std::string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " -o " + targetNick + "\r\n";
    channel->sendAllClient(modeMsg);
  }
}

void Event::_manageMode_L(string &message, Client &client) {
  string channelName, mode, param;
  modeParamParsing(message, channelName, mode, param);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL) {
    string msg = ERR_NOSUCHCHANNEL(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->findClient(client.getSocket()) == false) {
    string msg = ERR_NOTONCHANNEL(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }
  if (channel->clientIsOperator(client) == false) {
    string msg = ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName);
    sendRC(msg, client.getSocket());
    return;
  }

  if (mode[0] == '+') {
    if (param.empty()) {
      string msg = ERR_NEEDMOREPARAMS(client.getNickname(), "MODE");
      sendRC(msg, client.getSocket());
      return;
    }
    int limit = convertIntSafe(param.c_str());
    // if (limit <= 0) {
    //   string msg = ERR_INVALIDLIMIT(client.getNickname(), channelName);
    //   sendRC(msg, client.getSocket());
    //   return;
    // }
    channel->setUserLimit(limit);
  } else if (mode[0] == '-') {
    channel->removeUserLimit();
  }

  string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " " + mode + (param.empty() ? "" : " " + param) + "\r\n";
  channel->sendAllClient(modeMsg);
}
