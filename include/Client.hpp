#ifndef __CLIENT__
#define __CLIENT__

#include "libirc.hpp"
#include <netinet/in.h>

#define MAX_CLIENT_NAME 9

class IRC;

class Client {

public:
  Client(int, sockaddr_in, string);
  ~Client();

  int getSocket() const;
  const string &getIp() const;
  uint16_t getPort() const;
  const string &getNickname() const;
  const string &getUniqId() const;
  void changeNickname(string &);
  void handleMessage(string, IRC &server);

  bool operator==(const Client &) const;

private:
  void configMessage(string &, IRC &server);
  int receiveMessage(string &, IRC &server);
  bool correctNickFormat(string &);

  static IRC &serv;
  int _socket;
  string _nickname;
  string _hostname;
  string _username;
  string _uniqId;

  bool _isValidate;
  bool _isConnected;
  bool _badConfig;

  sockaddr_in _clientAdress;
  string _ip;

  vector<string> _messageTmp;
};

#endif
