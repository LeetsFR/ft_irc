#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"

class Client;

class Channel {

public:
  Channel(const string &name);
  ~Channel();

  const string &getName() const;
  void joinChannel();
  void sendMessage(const string &);
  bool clientIsOperator(Client &);
  void kickClient(Client &);

private:
  string &_name;
  map<Client, bool> _listClient;
};

#endif
