#ifndef __EVENT__
#define __EVENT__

#include "IRC.hpp"
#include "libirc.hpp"

class Event {

public:
  Event(string &message, Client &client, typeMsg type, IRC &serv);
  ~Event();

private:
  IRC &_serv;
  void _managePING(Client &client);
  void _manageKICK(string &message, Client &client);
  void _createChannel(string &message);
};

#endif
