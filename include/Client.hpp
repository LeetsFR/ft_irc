#ifndef __CLIENT__
#define __CLIENT__

#include "header.hpp"

#define MAX_CLIENT_NAME 9

class Client {

public:
  Client(const string &nickname, const string &hostname, const string &username);
  ~Client();

private:
  string _nickname;       // uniaue et fait 9 carateres
  const string _hostname; // le nom de la machine hote
  const string _username; // le nom du user sur la mahcine hote

  // inutile surement car il n'y a qu'un server
  string _servername;
  int _serverSocket;

  bool _isValidate;
};

#endif
