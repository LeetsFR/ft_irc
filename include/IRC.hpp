#ifndef __IRC__
#define __IRC__

#include "libft.hpp"
#include <sys/epoll.h>

#define MAX_EVENT 10

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

private:
  int _serverSocket = 0;
  int _port = 0;
  int _epollFd = 0;
  string _password;
  sockaddr_in _serverAdress;
  epoll_event _event;
  epoll_event _events[MAX_EVENT];


  bool _portIsValid();
  void _initSocket();
  void _initEpoll();
  void _acceptClient();
};

#endif
