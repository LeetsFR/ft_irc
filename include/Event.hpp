#ifndef __EVENT__
#define __EVENT__

#include "Client.hpp"
#include "IRC.hpp"
#include "libirc.hpp"
#include <locale>

class Event {

public:
  Event(string &message, Client &client, typeMsg type, IRC &serv);
  ~Event();

private:
  IRC &_serv;
  void _managePING(Client &client);
  void _manageKICK(string &message, Client &client);
  void _manageJOIN(string &message, Client &client);
  void _manageINVITE(string &message, Client &Client);
  void _manageTOPIC(string &message, Client &client);
  void _manageMode_I(string &message, Client &client);
  void _manageMode_T(string &message, Client &client);
  void _manageMode_K(string &message, Client &client);
  void _manageMode_O(string &message, Client &client);
  void _manageMode_L(string &message, Client &client);
};

#endif
