#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"

Channel::Channel(const string &name, const string &password, Client &client)
    : _name(name), _password(password) {
  _listClient.insert(make_pair(client, true));
  string err = RPL_NOTOPIC(client.getNickname(), _name);
  if (send(client.getSocket(), err.c_str(), err.size(), 0) == -1)
    cerr << printTime() << RED "Error: fail to send message" RESET << endl;
  _topic = "";
  _limitClient = 0;
  _actualNbrClient = 0;
  _limitClientMode = false;
  _inviteOnly = false;
  _topicOnlyOperator = false;
}

Channel::~Channel() {}

const string &Channel::getName() const { return _name; }

bool Channel::clientIsOperator(Client &client) {
  map<Client, bool>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first == client)
      return it->second;
  }
  throw logic_error("Error: Don't find Client clientIsOperator()");
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
  if (password != password) {
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
  string msg = RPL_NAMREPLY_AND_ENDOFNAMES(client.getNickname(), _name, _userList());
  sendRC(msg, client.getSocket());
  _listClient.insert(make_pair(client, true));
}

void Channel::kickClient(Client &client) {
  map<Client, bool>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->first == client)
      _listClient.erase(it);
  }
  throw logic_error("Error: Don't find Client kickClient()");
}

void Channel::sendMessage(const string &message) const {
  map<Client, bool>::const_iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (send(it->first.getSocket(), message.c_str(), message.size(), 0) == -1)
      cerr << printTime() << RED "Error: fail to send message" RESET << endl;
  }
}

string Channel::_userList() const {
  string userlist;

  map<Client, bool>::const_iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    //le cas ou c'est un operator je @ sinon je l'ecris juste 
  }
  return userlist;
}
