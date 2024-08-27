#include "IRC.hpp"
#include <sys/socket.h>

IRC::IRC(const string &port, const string &password) {

  _password = password;
  _port = convertIntSafe(port);
  if (_portIsValid() == false)
    throw logic_error("Error: Invalid port");
  _initSocket();
  _acceptClient();
}

IRC::~IRC() { close(_serverSocket); }

// Port a parser
bool IRC::_portIsValid() {
  if (_port == 6667)
    return true;
  return false;
}

void IRC::_acceptClient() {
  while (true) {
    int clientSocket =
        accept(_serverSocket, NULL,
               NULL); // On peut recup les infos client avec accept
    if (clientSocket == -1)
      throw logic_error("Error: Failed to accept the client socket");
    char buffer[100];
    recv(clientSocket, buffer, 100, 0);
    cout << "Message recu > " << buffer << endl;
  }
}

void IRC::_initSocket() {
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (_serverSocket == -1)
    throw logic_error("Error: Failed to create server socket");

  _serverAdress.sin_family = AF_INET;
  _serverAdress.sin_port = htons(_port);
  _serverAdress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (bind(_serverSocket, (struct sockaddr *)&_serverAdress, sizeof(_serverAdress)) == -1)
    throw logic_error("Error: Failed to bind the server socket");
  if (listen(_serverSocket, 5) == -1)
    throw logic_error("Error: Failed to listen the server socket");
  int opt = 1;
  if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    throw logic_error("Error: Failed to setsockopt the server socket");
}
