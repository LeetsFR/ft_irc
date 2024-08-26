#ifndef __IRC__
#define __IRC__

#include "header.hpp"
#include "Poll.hpp"

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

  int getSocket();
  void addClient(int client);
  void initSocketAdrr();
  void initSocket();
  void launchEpoll();

private:
  sockaddr_in _addr;
  string _password;
  int _port;
  int _fdSocket;
  Poll *epoll;
  vector<int> _tabClient;
};

#endif
