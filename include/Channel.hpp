#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"

class Client;

class Channel {

public:
  Channel(const string &name, const string &password, Client &);
  ~Channel();

  const string &getName() const;
  const bool getInviteOnly() const;
  const bool getProtectedTopic() const;
  const string &getTopic() const;
  const string getUserList() const;
  void setInviteOnly(bool);
  void setTopicOnlyOperator(bool);
  void setPassword(string &);
  void addInvitedClient(const string &);
  void modifyTopic(const string &topic);
  void joinChannel(const string &password, Client &client);
  bool clientIsOperator(Client &);
  void kickClient(Client &);
  bool isInvitedClient(const string &name) const;
  bool findClient(const int fd) const;
  const Client *findClient(const string &);
  void sendAllOtherClient(const string &, int) const;
  void sendAllClient(const string &) const;

private:
  const string _name;
  string _password;
  string _topic;
  bool _protectedTopic;
  int _limitClient;
  int _actualNbrClient;
  bool _limitClientMode;
  bool _inviteOnly;
  vector<string> _invitedClient;
  bool _topicOnlyOperator;
  map<Client, bool> _listClient;
};

#endif
