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

  void handleMessage(string, IRC &server);
  int getSocket();
  const string &getIp() const;
  uint16_t getPort() const;
  string getNickname();
  string getUniqId();
  void changeNickname(string &);

private:
  void configMessage(string &, IRC &server);
  void receiveMessage(string &, IRC &server);
  bool correctNickFormat(string &);

  static IRC& serv;
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
