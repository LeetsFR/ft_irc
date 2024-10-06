#include "IRC.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "libirc.hpp"

bool run = false;

IRC::IRC(const string &port, const string &password) {
  _name = "server";
  _password = password;
  _port = convertIntSafe(port);
  if (portIsValid(_port) == false)
    throw logic_error("Error: Invalid port");
  _initSocket();
  _initEpoll();
  _initSignal();
  cout << printTime() << GREEN "The server has been successfully initialized" RESET << endl;
  _waitEvent();
}

IRC::~IRC() {
  vector<Client>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    close(it->getSocket());
  }
  close(_serverSocket);
  close(_epollFd);
}

string IRC::getName() { return _name; }

int IRC::getSocket() { return _serverSocket; }

bool IRC::checkPassword(const string &password) { return password == _password; }

void IRC::printAllClient() const {
  vector<Client>::const_iterator it;
  cout << printTime() << "Client :" << endl;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    cout << "socket = " << it->getSocket() << endl;
    cout << "nickname = " << it->getNickname() << endl;
    cout << "hostname = " << it->getHostname() << endl;
  }
}

void IRC::printAllChannel() const {
  vector<Channel>::const_iterator it;
  cout << printTime() << "Channel:" << endl;
  for (it = _listChannel.begin(); it != _listChannel.end(); ++it) {
    cout << "name = " << it->getName() << endl;
  }
}

bool IRC::findNickname(const string &nickname) {
  vector<Client>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->getNickname() == nickname)
      return true;
  }
  return (false);
}

bool IRC::doesNicknameExist(const string &nickname) {
  vector<Client>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->getNickname() == nickname)
      return true;
  }
  return (false);
}

bool IRC::doesChannelExist(const string &channel) {
  vector<Channel>::iterator it;

  for (it = _listChannel.begin(); it != _listChannel.end(); ++it) {
    // cout << "GetName " + it->getName() << " channel " << channel << endl;
    if (it->getName() == channel)
      return true;
  }
  return (false);
}

Client &IRC::findClient(int fd) {
  vector<Client>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->getSocket() == fd)
      return *it;
  }
  throw logic_error("Error: Client not found");
}

Client *IRC::findClient(const string &clientName) {
  vector<Client>::iterator it;

  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->getNickname() == clientName)
      return &(*it);
  }
  return NULL;
}

Channel *IRC::findChannel(const string &channelName) {
  vector<Channel>::iterator it;

  for (it = _listChannel.begin(); it != _listChannel.end(); ++it) {
    if (it->getName() == channelName)
      return &(*it);
  }
  return NULL;
}

void IRC::createChannel(const string &name, const string &password, Client &client) { _listChannel.push_back(Channel(name, password, client)); }

void IRC::removeClient(int fd) {
  epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &_event);
  vector<Client>::iterator it;
  for (it = _listClient.begin(); it != _listClient.end(); ++it) {
    if (it->getSocket() == fd) {
      _listClient.erase(it);
      cout << printTime() << "(IP: " << it->getIp() << " - PORT: " << it->getPort() << ") Client disconnected" << endl;
      close(fd);
      return;
    }
  }
  throw logic_error("Error: Failed to find client");
}

void IRC::_addNewClient() {
  sockaddr_in clientAddress;
  int addrlen = sizeof(clientAddress);
  int clientSocket = accept(_serverSocket, (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
  if (clientSocket == -1)
    throw logic_error("Error: Failed to accept the client socket");

  char ip[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &(clientAddress.sin_addr), ip, INET_ADDRSTRLEN) == NULL)
    throw logic_error("Error: Failed to use inet_ntop");
  if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
    throw logic_error("Error: Failed to set client socket to non-blocking mode");

  _event.data.fd = clientSocket;
  epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &_event);
  _listClient.push_back(Client(clientSocket, clientAddress, ip));
  cout << printTime() << "(IP: " << ip << " - PORT: " << ntohs(clientAddress.sin_port) << ") Client connected" << endl;
}

void IRC::_getEventClient(int fd) {
  string message;

  if (getMessage(fd, message) == false) {
    removeClient(fd);
    return;
  }
  cout << printTime() << message << endl;
  Client &client = findClient(fd);
  if (client.handleMessage(message, *this) == false) {
    removeClient(fd);
    return;
  }
}

void IRC::_waitEvent() {
  while (run) {
    int numberEvents = epoll_wait(_epollFd, _events, MAX_EVENT, -1);
    if (numberEvents == -1) {
      if (errno != EINTR)
        throw logic_error("Error: Failed to accept the client socket");
      cout << printTime() << GREEN << "Server has shut down normally" << RESET << endl;
    }
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

void IRC::_initSignal() {
  run = true;
  signal(SIGINT, handleSigint);
}
