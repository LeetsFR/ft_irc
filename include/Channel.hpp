#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"
#include <vector>

class Client;

class Channel {

public:
  Channel(const string &name, const string &password, Client &);
  ~Channel();

  const string &getName() const;
  void joinChannel(const string &password, Client &client);
  void sendMessage(const string &) const;
  bool clientIsOperator(Client &);
  void kickClient(Client &);
  bool isInvitedClient(const string &name) const;

private:
  const string _name;
  string _password;
  string _topic;
  int _limitClient;
  int _actualNbrClient;
  bool _limitClientMode;
  bool _inviteOnly;
  vector<string> _invitedClient;
  bool _topicOnlyOperator;
  map<Client, bool> _listClient;

  string _userList() const;
};

#endif
