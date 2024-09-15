#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"

class Client;

class Channel {

public:
  Channel(const string &name, const string &password, Client &);
  ~Channel();

  const string &getName() const;
  const string getUserList() const;
  void joinChannel(const string &password, Client &client);
  bool clientIsOperator(Client &);
  void kickClient(Client &);
  void sendAllOtherClient(const string &, int) const;
  void sendAllClient(const string &) const;
  bool isInvitedClient(const string &name) const;
  Client *findClient(const string &) const;

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
};

#endif
