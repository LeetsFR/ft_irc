#include "Poll.hpp"

Poll::Poll(int fdserver) {
  this->_epoll.events = EPOLLIN;
  this->_epoll.data.fd = fdserver;
  this->_fdpoll = epoll_create(1);
  this->_fdserver = fdserver;
  if (this->_fdpoll == -1)
    throw logic_error("Error: epoll_create() failed");
  if (epoll_ctl(this->_fdpoll, EPOLL_CTL_ADD, fdserver, &this->_epoll) != 0)
    throw logic_error("Error: epoll_ctl() failed");
}

Poll::~Poll(){
  close(this->_fdpoll);
}

int Poll::setClient(int severSocket) {
  sockaddr_in clientAddress;
  int addrlen = sizeof(clientAddress);
  int clientSocket = accept(severSocket, (struct sockaddr *)&clientAddress, (socklen_t *)&addrlen);
  if (clientSocket != -1) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), ip, INET_ADDRSTRLEN);
    cout << GREEN "Success: Connection from " << clientAddress.sin_zero << endl;
    cout << ip << ":" << ntohs(clientAddress.sin_port) << RESET << "\n";
    int opt = 1;
    setsockopt(clientSocket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
  }
  return (clientSocket);
}

void Poll::launchPoll() {
  int fdclient;

  while (true) {
    int nfds = epoll_wait(this->_fdpoll, this->_event, 10, -1);
    if (nfds == -1)
      throw logic_error("Error: epoll_wait() failed");
    for (int i = 0; i < nfds; ++i) {
      if (this->_event[i].data.fd == this->_fdserver) {
        fdclient = setClient(this->_fdserver);
        if (fdclient == -1)
          throw logic_error("Error: setClient() failed");
        this->_event[i].data.fd = fdclient;
        epoll_event client_event; // Utilisez un nouvel objet pour le nouvel ajout
    client_event.events = EPOLLIN; // ou EPOLLIN | EPOLLOUT selon le besoin
    client_event.data.fd = fdclient;

        if (epoll_ctl(this->_fdpoll, EPOLL_CTL_ADD, fdclient,&client_event)) {
          throw logic_error("Error: epoll_ctl() failed");
        }
      } else {
        char buffer[1024];
        string msg;
        int size_read = read(this->_event[i].data.fd, buffer, sizeof(buffer));
        if (size_read < 0)
          throw logic_error("Error: read() failed");
        if (size_read == 0) {
          cout << "Client disconnected: " << this->_event[i].data.fd << endl;
          close(this->_event[i].data.fd);
          epoll_ctl(this->_fdpoll, EPOLL_CTL_DEL, this->_event[i].data.fd, NULL);
          return;
        }
        string str(buffer, size_read);
        if (str == "CAP LS") {
          msg = ":shining CAP * LS :multi-prefix\r\n";
          if (send(this->_event[i].data.fd, msg.c_str(), msg.size(), 0) == -1)
            throw logic_error("Error: send() failed");
        }
        if (str == "CAP REQ :multi-prefix") {
          string msg = ":shining CAP * ACK :multi-prefix\r\n";
          if (send(this->_event[i].data.fd, msg.c_str(), msg.size(), 0) == -1)
            throw logic_error("Error: send() failed");
        }
        if (str == "CAP END") {
          msg = ":shining 001 dan :Welcome to the "
                "Internet Relay Network dan\r\n";
          if (send(this->_event[i].data.fd, msg.c_str(), msg.size(), 0) == -1)
            throw logic_error("Error: send() failed");
        }
        cout << "Received message from client: " << this->_event[i].data.fd << endl;
        cout << buffer << endl;
      }
    }
  }
}
