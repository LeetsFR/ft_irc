#ifndef __CHANNEL__
#define __CHANNEL__

#include "libirc.hpp"

class Client;

class Channel {

  public:
    Channel(const string& name);
    ~Channel();

    void joinChannel();
    void sendMessage();

  private:
    string& _name;
    map<Client,bool> _listClient;
      
};

#endif
