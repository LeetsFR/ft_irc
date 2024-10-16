#include "Event.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "IRC.hpp"
#include "libirc.hpp"
#include <csignal>
#include <sys/socket.h>
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
    _manageKICK(message, client);
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

Event::~Event() {}

void Event::_managePRIVMSG(string &message, Client &client) {
  string targetName;
  string msgContent;

  privmsgParsing(message, targetName, msgContent);

  if ((targetName[0] == '#' || targetName[0] == '&')) {
    Channel *channel = _serv.findChannel(targetName);
    if (channel == NULL)
      return sendRC(ERR_NOSUCHCHANNEL(client.getNickname(), targetName), client.getSocket());

    if (channel->findClient(client.getSocket()) == false)
      return sendRC(ERR_CANNOTSENDTOCHAN(client.getNickname(), targetName), client.getSocket());
    channel->sendAllOtherClient(PRIVMSG(client.getNickname(), targetName, msgContent), client.getSocket());
  } else {
    Client *targetClient = _serv.findClient(targetName);
    if (targetClient == NULL)
      return sendRC(ERR_NOSUCHNICK(client.getNickname(), targetName), client.getSocket());

    if (targetClient->getSocket() != client.getSocket())
      sendRC(PRIVMSG(client.getNickname(), targetClient->getNickname(), msgContent), targetClient->getSocket());
  }
}

void Event::_managePING(Client &client) { sendRC(REP_PONG(client.getNickname()), client.getSocket()); }

void Event::_manageKICK(string &message, Client &client) {
  string channelName, kickUserName, reason;

  kickParsing(message, channelName, kickUserName, reason);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getNickname(), channelName), client.getSocket());
  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName), client.getSocket());

  Client *kickUser = _serv.findClient(kickUserName);
  if (kickUser == NULL)
    return sendRC(ERR_NOSUCHNICK(client.getNickname(), kickUserName), client.getSocket());
  if (channel->findClient(kickUser) == false)
    return sendRC(ERR_USERNOTINCHANNEL(client.getNickname(), kickUser->getNickname(), channel->getName()),
                  client.getSocket());

  string kickMsg = ":"+ client.getNickname() + "@server KICK " + channelName + " " + kickUserName + " :" + reason + "\r\n";
  channel->sendAllClient(kickMsg);
  channel->kickClient(kickUser);
}

void Event::_manageTOPIC(string &message, Client &client) {
  string channelName, topic, msg;

  bool isTopicIsChanged = topicParsing(message, channelName, topic);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getHostname(), channelName), client.getSocket());

  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getHostname(), channelName), client.getSocket());

  if (isTopicIsChanged == true) {
    if (channel->getTopicOnlyOperator() == true) {
      if (channel->clientIsOperator(client) == false)
        return sendRC(ERR_CHANOPRIVSNEEDED(client.getNickname(), channel->getName()), client.getSocket());
    }
    channel->modifyTopic(topic);
    string topicMsg = ":" + client.getNickname() + "@server TOPIC " + channelName + " :" + topic + "\r\n";
    return channel->sendAllClient(topicMsg);
  } else {
    if (channel->getTopic().empty())
      msg = RPL_NOTOPIC(client.getHostname(), channel->getName());
    else
      msg = RPL_TOPIC(client.getHostname(), channel->getName(), channel->getTopic());
  }

  return sendRC(msg, client.getSocket());
}

void Event::_manageJOIN(string &message, Client &client) {

  string channelName;
  string password;
  joinParsing(message, channelName, password);
  Channel *channelPtr = _serv.findChannel(channelName);
  if (channelPtr == NULL)
    _serv.createChannel(channelName, password, client);
  else
    channelPtr->joinChannel(password, client);
}

void Event::_manageINVITE(string &message, Client &client) {
  string clientName, channelName;
  inviteParsing(message, clientName, channelName);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getNickname(), channelName), client.getSocket());

  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getNickname(), channelName), client.getSocket());

  Client *invitedClient = _serv.findClient(clientName);
  if (invitedClient == NULL)
    return sendRC(ERR_NOSUCHNICK(client.getNickname(), clientName), client.getSocket());

  if (channel->findClient(invitedClient->getSocket()))
    return sendRC(ERR_USERONCHANNEL(client.getNickname(), clientName), client.getSocket());

  if (channel->getInviteOnly()) {
    if (channel->clientIsOperator(client) == false)
      return sendRC(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName), client.getSocket());
  }
  channel->addInvitedClient(invitedClient->getNickname());
  sendRC(RPL_INVITING(client.getNickname(), invitedClient->getNickname(), channelName), client.getSocket());

  string inviteMsg = ":" + client.getNickname() + "@server INVITE " + invitedClient->getNickname() + " :" + channelName + "\r\n";
  sendRC(inviteMsg, invitedClient->getSocket());
}

void Event::_manageMode_I(string &message, Client &client) {
  string channelName, mode;
  modeParsing(message, channelName, mode);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getHostname(), channelName), client.getSocket());
  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getHostname(), channel->getName()), client.getSocket());
  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName()), client.getSocket());
  if (mode[0] == '+')
    channel->setInviteOnly(true);
  else if (mode[0] == '-')
    channel->setInviteOnly(false);
}

void Event::_manageMode_T(string &message, Client &client) {
  string channelName, mode;
  modeParsing(message, channelName, mode);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getHostname(), channelName), client.getSocket());
  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getHostname(), channel->getName()), client.getSocket());
  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName()), client.getSocket());
  if (mode[0] == '+')
    channel->setTopicOnlyOperator(true);
  else if (mode[0] == '-')
    channel->setTopicOnlyOperator(false);
}

void Event::_manageMode_K(string &message, Client &client) {
  string channelName, mode, param;
  modeParamParsing(message, channelName, mode, param);
  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getHostname(), channelName), client.getSocket());
  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getHostname(), channel->getName()), client.getSocket());
  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getHostname(), channel->getName()), client.getSocket());
  if (mode[0] == '+')
    channel->setPassword(param, true);
  else if (mode[0] == '-') {
    string tmp = "";
    channel->setPassword(tmp, false);
  }
}

void Event::_manageMode_O(string &message, Client &client) {
  string channelName, mode, targetNick;
  modeParamParsing(message, channelName, mode, targetNick);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getHostname(), channelName), client.getSocket());

  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getHostname(), channelName), client.getSocket());

  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName), client.getSocket());
  Client *targetClient = _serv.findClient(targetNick);
  if (targetClient == NULL)
    return sendRC(ERR_NOSUCHNICK(client.getNickname(), targetNick), client.getSocket());

  if (channel->findClient(targetClient->getSocket()) == false)
    return sendRC(ERR_USERNOTINCHANNEL(client.getNickname(), targetNick, channelName), client.getSocket());
  if (mode[0] == '+') {
    if (channel->clientIsOperator(*targetClient))
      return sendRC(ERR_USERONCHANNEL(client.getNickname(), targetNick), client.getSocket());
    channel->addOperator(targetClient);
    string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " +o " + targetNick + "\r\n";
    channel->sendAllClient(modeMsg);

  } else if (mode[0] == '-') {
    if (channel->clientIsOperator(*targetClient) == false)
      return sendRC(ERR_USERNOTINCHANNEL(client.getNickname(), targetNick, channelName), client.getSocket());
    channel->removeOperator(*targetClient);
    string modeMsg = ":" + client.getNickname() + " MODE " + channelName + " -o " + targetNick + "\r\n";
    channel->sendAllClient(modeMsg);
  }
}

void Event::_manageMode_L(string &message, Client &client) {
  string channelName, mode, param;
  modeParamParsing(message, channelName, mode, param);

  Channel *channel = _serv.findChannel(channelName);
  if (channel == NULL)
    return sendRC(ERR_NOSUCHCHANNEL(client.getNickname(), channelName), client.getSocket());
  if (channel->findClient(client.getSocket()) == false)
    return sendRC(ERR_NOTONCHANNEL(client.getNickname(), channelName), client.getSocket());
  if (channel->clientIsOperator(client) == false)
    return sendRC(ERR_CHANOPRIVSNEEDED(client.getNickname(), channelName), client.getSocket());
  if (mode[0] == '+') {
    if (param.empty())
      return sendRC(ERR_NEEDMOREPARAMS(client.getNickname(), "MODE"), client.getSocket());
    int limit = convertIntSafe(param.c_str());
    if (limit <= 0) 
      return sendRC(ERR_INVALIDLIMIT(client.getNickname(), channelName), client.getSocket());
    channel->setUserLimit(limit);
  } else if (mode[0] == '-') {
    channel->removeUserLimit();
  }
  // string modeMsg = ":" + client.getNickname() + "!" + client.getUser() + "@" + client.getHostname() +
  //                " MODE " + channelName + " " + mode + (param.empty() ? "" : " " + param) + "\r\n";
  string modeMsg = ":" + client.getNickname() + "@server MODE " + channelName + " " + mode + (param.empty() ? "" : " " + param) + "\r\n";
  channel->sendAllClient(modeMsg);
}
