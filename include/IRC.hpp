#ifndef __IRC__
#define __IRC__

#include "libirc.hpp"

#define MAX_EVENT 200

class Client;

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

  string getName();
  int getSocket();
  bool checkPassword(const string &);
  bool findNickname(const string &);
  Client &findClient(int fd);
  void removeClient(int fd);


private:
  string _name;
  int _serverSocket;
  int _port;
  int _epollFd;
  string _password;
  sockaddr_in _serverAdress;
  epoll_event _event;
  epoll_event _events[MAX_EVENT];
  vector<Client> _listClient;

  void _initSocket();
  void _initEpoll();
  void _initSignal();
  void _waitEvent();
  void _addNewClient();
  void _getEventClient(int fd);
};

extern bool run;

#endif
