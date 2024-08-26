#include "IRC.hpp"
#include <atomic>
#include <cstring>

int setClient(int severSocket) {
  int clientSocket;
  sockaddr_in clientAddress;
  int addrlen = sizeof(clientAddress);
  clientSocket = accept(severSocket, (struct sockaddr *)&clientAddress,
                        (socklen_t *)&addrlen);
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

int main(void) {
  IRC server("6667", "42");
  struct epoll_event ev_epoll;
  ev_epoll.events = EPOLLIN;
  ev_epoll.data.fd = server.getSocket();
  struct epoll_event event[200];

  int fdEpoll = epoll_create(1);
  if (fdEpoll == -1) {
    cerr << RED "epoll_create()" << endl;
    return 0;
  }
  if (epoll_ctl(fdEpoll, EPOLL_CTL_ADD, server.getSocket(), &ev_epoll)) {
    cerr << RED "epoll_ctl()" << endl;
    return 0;
  }
  while (true) {
    int nfds = epoll_wait(fdEpoll, event, 10, -1);
    if (nfds == -1) {
      cerr << RED "epoll_create()" << endl;
      return 1;
    }
    int clientSocket;
    for (int i = 0; i < nfds; ++i) {
      if (event[i].data.fd == server.getSocket()) {
        clientSocket = setClient(server.getSocket());
        if (clientSocket != -1) {
          server.addClient(clientSocket);
          ev_epoll.data.fd = clientSocket;
          if (epoll_ctl(fdEpoll, EPOLL_CTL_ADD, clientSocket, &ev_epoll)) {
            cout << "epoll_ctl (add client)\n";
            return (1);
          }
        }
      }

      else {
        char buffer[1024];
        int nb_read = read(event[i].data.fd, buffer, sizeof(buffer));
        if (nb_read > 0) {
          if (!strncmp(buffer, "CAP LS", 6)) {
            const string msg_1 = ":shining CAP * LS :multi-prefix\r\n";
            if (send(event[i].data.fd, msg_1.c_str(), msg_1.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
          }
          if (!strncmp(buffer, "CAP REQ :multi-prefix", 21)) {
            const string msg_2 = ":shining CAP * ACK :multi-prefix\r\n";
            if (send(event[i].data.fd, msg_2.c_str(), msg_2.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
          }
          if (!strncmp(buffer, "CAP END", 7)) {
            const string msg_3 = ":shining 001 dan :Welcome to the "
                                 "Internet Relay Network dan\r\n";
            if (send(event[i].data.fd, msg_3.c_str(), msg_3.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
          }
          if (!strncmp(buffer, "PRIVMSG", 7)) 
          {
            const string msg_4 = ":sam PRIVMSG #samuel : wapwap\r\n";
            if (send(event[i].data.fd, msg_4.c_str(), msg_4.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
          }
          cout << "Received message from client: " << event[i].data.fd << endl;
          buffer[nb_read] = '\0';
          // cout << buffer << endl;
          for (size_t i = 0; i < strlen(buffer); i++)
              std::cout << (int)buffer[i] << "|" << buffer[i] << "\n";
        } else if (nb_read == 0) {
          cout << "Client disconnected: " << event[i].data.fd << "\n";
          close(event[i].data.fd);
          epoll_ctl(fdEpoll, EPOLL_CTL_DEL, event[i].data.fd, NULL);
        }
      }
    }
  }

  close(fdEpoll);
  close(server.getSocket());
}
