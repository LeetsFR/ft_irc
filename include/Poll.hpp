#ifndef __POLL__
#define __POLL__

#include "header.hpp"

class Poll {

public:
  Poll(int fdserver);
  ~Poll();

  void launchPoll();

private:
  epoll_event _param;
  epoll_event _event[10];
  int _fdpoll;
  int _fdserver;

};

#endif
