#include "IRC.hpp"
#include "libirc.hpp"
#include <cerrno>

IRC::IRC(const string &port, const string &password) {
  _password = password;
  _port = convertIntSafe(port);
  if (portIsValid(_port) == false)
    throw logic_error("Error: Invalid port");
  _initSocket();
  _initEpoll();
  _initSignal();
  _waitEvent();
}

IRC::~IRC() {
  close(_serverSocket);
  close(_epollFd);
}

void IRC::_initSignal() { signal(SIGINT, handleSigint); }

void IRC::_addNewClient() {
  int clientSocket = accept(_serverSocket, NULL, NULL);
  if (clientSocket == -1 && errno != EINTR)
    throw logic_error("Error: Failed to accept the client socket");
  else {
    cout << "Shutting down the server..." << endl;
  }
  if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
    throw logic_error("Error: Failed to set client socket to non-blocking mode");
  _event.data.fd = clientSocket;
  epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &_event);
}

void IRC::_getEventClient(int fd) {
  string message;

  if (getMessage(fd, message) == false) {
    epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &_event);
    close(fd);
  }
}

void IRC::_waitEvent() {

  while (true) {
    int numberEvents = epoll_wait(_epollFd, _events, MAX_EVENT, -1);
    if (numberEvents == -1)
      throw logic_error("Error: Failed to accept the client socket");
    for (int i = 0; i < numberEvents; ++i) {
      if (_events[i].data.fd == _serverSocket)
        _addNewClient();
      else
        _getEventClient(_events[i].data.fd);
    }
  }
}

void IRC::_initEpoll() {
  _epollFd = epoll_create(1);
  if (_epollFd == -1)
    throw logic_error("Error: Failed to create epoll fd");
  _event.events = EPOLLIN;
  _event.data.fd = _serverSocket;
  if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverSocket, &_event) == -1)
    throw logic_error("Error: Failed to epoll_ctl");
}

void IRC::_initSocket() {
  _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (_serverSocket == -1)
    throw logic_error("Error: Failed to create server socket");
  if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1)
    throw logic_error("Error: Failed to set server socket to non-blocking mode");

  _serverAdress.sin_family = AF_INET;
  _serverAdress.sin_port = htons(_port);
  _serverAdress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  int opt = 1;
  if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    throw logic_error("Error: Failed to setsockopt the server socket");
  if (bind(_serverSocket, (struct sockaddr *)&_serverAdress, sizeof(_serverAdress)) == -1)
    throw logic_error("Error: Failed to bind the server socket");
  if (listen(_serverSocket, 5) == -1)
    throw logic_error("Error: Failed to listen the server socket");
}
