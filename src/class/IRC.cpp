#include "IRC.hpp"

IRC::IRC(const string &port, const string &password) {

  _password = password;
  _port = convertIntSafe(port);
  if (portIsValid() == false)
    throw logic_error("Error: Invalid port");
  initSocket();
}

IRC::~IRC() { close(_serverSocket); }

// Port a parser
bool IRC::portIsValid() {
  if (_port == 6667)
    return true;
  return false;
}

void IRC::initSocket() {
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
  cout << "SERVEUR IS OPEN " << endl;
  int clientSocket = accept(_serverSocket, NULL, NULL);
  char buffer[1024] = {0};
  recv(clientSocket, buffer, sizeof(buffer), 0);
  cout << "Message from client: " << buffer << endl;
}
