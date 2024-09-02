#include "header.hpp"
#include "Client.hpp"
#include "IRC.hpp"

bool g_stop = false;

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
    handler_signal();
    try
    {
        IRC server("6667", "42");
        struct epoll_event ev_epoll;
        ev_epoll.events = EPOLLIN;
        ev_epoll.data.fd = server.getSocket();
        struct epoll_event event[200];

        int fdEpoll = epoll_create(1);
        if (fdEpoll == -1) {
            cerr << RED "epoll_create()" RESET << endl;
            return 0;
        }
        if (epoll_ctl(fdEpoll, EPOLL_CTL_ADD, server.getSocket(), &ev_epoll)) {
            cerr << RED "epoll_ctl()" RESET << endl;
            return 0;
        }
        while (!g_stop) {
            int nfds = epoll_wait(fdEpoll, event, 10, -1);
            if (nfds == -1) {
            cerr << RED "epoll_wait()" RESET << endl;
            return 1;
            }
            int clientSocket;
            for (int i = 0; i < nfds; ++i) {
            if (event[i].data.fd == server.getSocket()) {
                clientSocket = setClient(server.getSocket());
                if (clientSocket != -1) {
                cout << clientSocket << endl;
                server.addClient((clientSocket));
                ev_epoll.data.fd = clientSocket;
                if (epoll_ctl(fdEpoll, EPOLL_CTL_ADD, clientSocket, &ev_epoll)) {
                    cout << "epoll_ctl (add client)\n";
                    return (1);
                }
                }
            } else {
                char buffer[1024];
                int nb_read = read(event[i].data.fd, buffer, sizeof(buffer));
                Client *Isclient = server.findClient(event[i].data.fd);
                if (nb_read > 0) {
                buffer[nb_read] = '\0';
                cout << "Received message from client: " << event[i].data.fd << endl;
                cout << Isclient->getSocket() << endl; 
                Isclient->handleMessage(buffer, server);
                cout << buffer << endl;
                } else if (nb_read == 0) {
                cout << "Client disconnected: " << event[i].data.fd << "\n";
                server.deleteClient(clientSocket);
                epoll_ctl(fdEpoll, EPOLL_CTL_DEL, event[i].data.fd, NULL);
                }
            }
            }
        }
        cout << "C signal active " << g_stop << endl;
        close(fdEpoll);
        close(server.getSocket());
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

