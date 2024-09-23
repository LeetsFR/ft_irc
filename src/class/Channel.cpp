#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"
#include <vector>

Channel::Channel(const string &name, const string &password, Client &client) : _name(name), _password(password) {
  _listClient.insert(make_pair(client, true));
  string err = RPL_NOTOPIC(client.getNickname(), _name);
  if (send(client.getSocket(), err.c_str(), err.size(), 0) == -1)
    cerr << printTime() << RED "Error: fail to send message" RESET << endl;
  _topic = "";
  _protectedTopic = false;
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

bool Channel::getProtectedTopic() const { return _protectedTopic; }

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

void Channel::setPassword(string &password) { password = _password; }

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
  if (_inviteOnly == true) {
    if (isInvitedClient(client.getNickname()) == false) {
      string msg = ERR_INVITEONLYCHAN(client.getNickname(), _name);
      sendRC(msg, client.getSocket());
      return;
    }
  }
  if (_password != password) {
    string msg = ERR_BADCHANNELKEY(client.getNickname(), _name);
    sendRC(msg, client.getSocket());
    return;
  }
  if (_limitClientMode == true) {
    if (_actualNbrClient >= _limitClient) {
      string msg = ERR_CHANNELISFULL(client.getNickname(), _name);
      sendRC(msg, client.getSocket());
      return;
    } else
      ++_actualNbrClient;
  }
  if (_topic == "") {
    string msg = RPL_NOTOPIC(client.getNickname(), _name);
    sendRC(msg, client.getSocket());
  } else {
    string msg = RPL_TOPIC(client.getNickname(), _name, _topic);
    sendRC(msg, client.getSocket());
  }
  string msg = RPL_NAMREPLY_AND_ENDOFNAMES(client.getNickname(), _name, getUserList());
  sendRC(msg, client.getSocket());
  _listClient.insert(make_pair(client, true));
  string joinMsg = ":" + client.getNickname() + " JOIN :" + _name;
  sendAllOtherClient(joinMsg, client.getSocket());
}

void Channel::kickClient(Client &client) {
  map<Client, bool>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first == client)
      _listClient.erase(it);
  }
  throw logic_error("Error: Don't find Client kickClient()");
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
