#ifndef __IRC__
#define __IRC__

#include "libft.hpp"

class IRC {

public:
  IRC(const string &port, const string &password);
  ~IRC();

private:
  int _serverSocket = 0;
  int _port = 0;
  string _password;
  sockaddr_in _serverAdress;

  bool _portIsValid();
  void _initSocket();
  void _acceptClient();
};

#endif
