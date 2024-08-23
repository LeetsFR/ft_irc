#ifndef __HEADER__
#define __HEADER__

//C++
#include <iostream>
#include <string>
#include <vector>
#include <map>

//C 
#include <sys/socket.h> // socket, setsockopt, getsockname, bind, connect, listen, accept, send, recv
#include <unistd.h>     // close, lseek select
#include <netdb.h>      // getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo
#include <arpa/inet.h>  // htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa
#include <signal.h>     // signal, sigaction
#include <sys/stat.h>   // fstat
#include <fcntl.h>      // fcntl
#include <poll.h>       // poll
#include <sys/epoll.h>  // epoll

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

using namespace std;



#endif
