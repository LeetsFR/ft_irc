#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"

Channel::Channel(const string &name) : _name(name) {}

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
