#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"

class Client;

class Channel {

public:
  Channel(const string &name,Client);
  Channel(const string &name,const string &password,Client);
  ~Channel();

  const string &getName() const;
  void joinChannel();
  void sendMessage(const string &) const;
  bool clientIsOperator(Client &);
  void kickClient(Client &);

private:
  const string _name;
  const string _password;
  map<Client, bool> _listClient;
};

#endif
