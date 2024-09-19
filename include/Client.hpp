#ifndef __CLIENT__
#define __CLIENT__

#include "libirc.hpp"

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
  const string &getHostname() const;
  const string &getUniqId() const;
  const string &getUser() const;
  void handleMessage(string, IRC &server);

  bool operator==(const Client &) const;
  bool operator<(const Client &) const;

private:
  void configMessage(string &, IRC &server);
  typeMsg receiveMessage(string &, IRC &server);
  void sendMsgToClient(const std::string &message);

  bool correctNickFormat(string &);

  typeMsg parsPrivmsg(string &, IRC &server);
  typeMsg parsJoin(string &);
  typeMsg parsPing(string &);
  typeMsg parsKick(string &, IRC &server);
  typeMsg parsInvite(string &, IRC &server);
  typeMsg parsTopic(string &, IRC &server);
  typeMsg parsMode(string &, IRC &server);

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
  bool _prevMsgIncomplete;
};

std::ostream &operator<<(std::ostream &os, const Client &obj);

#endif
