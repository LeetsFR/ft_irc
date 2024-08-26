#ifndef __IRC__
#define __IRC__

#include "header.hpp"

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

  int getSocket();
  void addClient(int client);
  void initSocketAdrr();
  void initSocket();

private:
  sockaddr_in   _addr;
  string        _password;
  int           _port;
  int           _fdSocket;
  vector<int>   _tabClient;
  // vector<Client*>   _tabClient2;
};

#endif
