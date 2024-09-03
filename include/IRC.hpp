#ifndef __IRC__
#define __IRC__

#include "libirc.hpp"

#define MAX_EVENT 10

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

private:
  int _serverSocket;
  int _port;
  int _epollFd;
  string _password;
  sockaddr_in _serverAdress;
  epoll_event _event;
  epoll_event _events[MAX_EVENT];

  void _initSocket();
  void _initEpoll();
  void _initSignal();
  void _waitEvent();
  void _addNewClient();
  void _getEventClient(int fd);
};

#endif
