#ifndef __IRC__
#define __IRC__

#include "libirc.hpp"

#define MAX_EVENT 200

class Client;

class Channel;

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

  string getName();
  int getSocket();
  bool checkPassword(const string &);
  void printAllClient() const;
  void printAllChannel() const;
  bool findNickname(const string &);
  bool doesChannelExist(const string &);
  bool doesNicknameExist(const string &);
  Client &findClient(int fd);
  Client *findClient(const string &);
  Channel *findChannel(const string &name);
  void createChannel(const string &name, const string &password, Client &);
  void joinChannel(Channel *channel, const string &password, Client &);
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
  vector<Channel> _listChannel;

  void _initSocket();
  void _initEpoll();
  void _initSignal();
  void _waitEvent();
  void _addNewClient();
  void _getEventClient(int fd);
  void _removeClientChannel(int fd);
};

extern bool run;

#endif
