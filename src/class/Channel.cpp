#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"
#include <vector>

Channel::Channel(const string &name, const string &password, Client &client) : _name(name), _password(password) {
  _listClient.insert(make_pair(client, true));
  sendRC(RPL_NOTOPIC(client.getNickname(), _name), client.getSocket());
  _topic = "";
  if (password.empty())
    _passwordON = false;
  else
    _passwordON = true;
  _limitClient = 0;
  _actualNbrClient = 0;
  _limitClientMode = false;
  _inviteOnly = false;
  _topicOnlyOperator = false;
}

Channel::~Channel() {}

const string &Channel::getName() const { return _name; }

const string &Channel::getTopic() const { return _topic; }

bool Channel::getInviteOnly() const { return _inviteOnly; }

bool Channel::getTopicOnlyOperator() const { return _topicOnlyOperator; }

void Channel::modifyTopic(const string &topic) { _topic = topic; }

bool Channel::clientIsOperator(Client &client) {
  map<Client, bool>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first == client)
      return it->second;
  }
  throw logic_error("Error: Don't find Client clientIsOperator()");
}

void Channel::setInviteOnly(bool value) { _inviteOnly = value; }

void Channel::setTopicOnlyOperator(bool value) { _topicOnlyOperator = value; }

void Channel::setPassword(string &password, bool passwordON) {
  _passwordON = passwordON;
  password = _password;
}

void Channel::setUserLimit(int value) {
  _limitClientMode = true;
  _limitClient = value;
}

void Channel::removeUserLimit() { _limitClientMode = false; }

void Channel::addOperator(Client &client) {
  map<Client, bool>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getUniqId() == client.getUniqId())
      it->second = true;
  }
}

void Channel::removeOperator(Client &client) {
  map<Client, bool>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getUniqId() == client.getUniqId())
      it->second = false;
  }
}

void Channel::addInvitedClient(const string &invitedClientName) { _invitedClient.push_back(invitedClientName); }

const string Channel::getUserList() const {
  string userlist;

  map<Client, bool>::const_iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->second == true)
      userlist.append("@");
    userlist.append(it->first.getNickname());
    userlist.append(" ");
  }
  return userlist;
}

bool Channel::isInvitedClient(const string &name) const {
  vector<string>::const_iterator it;
  for (it = _invitedClient.begin(); it != _invitedClient.end(); ++it) {
    if (*it == name)
      return true;
  }
  return false;
}

void Channel::joinChannel(const string &password, Client &client) {
  if (_inviteOnly) {
    if (isInvitedClient(client.getNickname()) == false)
      return sendRC(ERR_INVITEONLYCHAN(client.getNickname(), _name), client.getSocket());
  }
  if (_passwordON == true) {
    if (_password != password)
      return sendRC(ERR_BADCHANNELKEY(client.getNickname(), _name), client.getSocket());
  }
  if (_limitClientMode) {
    if (_actualNbrClient >= _limitClient)
      return sendRC(ERR_CHANNELISFULL(client.getNickname(), _name), client.getSocket());
    ++_actualNbrClient;
  }

  _listClient.insert(make_pair(client, false));

  string joinMsg = ":" + client.getNickname() + "@server" + " JOIN :" + _name + "\r\n";
  sendRC(joinMsg, client.getSocket());

  if (_topic.empty()) {
    sendRC(RPL_NOTOPIC(client.getNickname(), _name), client.getSocket());
  } else {
    sendRC(RPL_TOPIC(client.getNickname(), _name, _topic), client.getSocket());
  }
  sendRC(RPL_NAMREPLY(client.getNickname(), _name, getUserList()), client.getSocket());
  sendRC(RPL_ENDOFNAMES(client.getNickname(), _name), client.getSocket());
  sendAllOtherClient(joinMsg, client.getSocket());
}

void Channel::kickClient(Client *client) {
  map<Client, bool>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getUniqId() == client->getUniqId())
      _listClient.erase(it);
    return;
  }
  cout << printTime() << "Error: Don't find Client kickClient() || " << endl;
}

void Channel::sendAllOtherClient(const string &message, int ignoredFd) const {
  map<Client, bool>::const_iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getSocket() != ignoredFd)
      sendRC(message, it->first.getSocket());
  }
}

bool Channel::findClient(const int fd) const {
  map<Client, bool>::const_iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getSocket() == fd)
      return true;
  }
  return false;
}

bool Channel::findClient(Client *client) const {
  map<Client, bool>::const_iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getSocket() == client->getSocket() && it->first.getUniqId() == client->getUniqId())
      return true;
  }
  return false;
}

const Client *Channel::findClient(const string &clientName) {
  map<Client, bool>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first.getNickname() == clientName) {
      return &it->first;
    }
  }
  return NULL;
}

void Channel::sendAllClient(const string &message) const {
  map<Client, bool>::const_iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (send(it->first.getSocket(), message.c_str(), message.size(), 0) == -1)
      cerr << printTime() << RED "Error: fail to send message" RESET << endl;
  }
}
