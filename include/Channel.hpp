#ifndef __CHANNEL__
#define __CHANNEL__

#include "Client.hpp"

class Channel {

public:
  Channel();
  ~Channel();

private:
  // 1er caratere & ou # et jusqu'a 200 caracteres
  // pas de espaces, virugle ou '\a'
  string _name;
  map<Client, bool> _connectClient;
};

#endif
