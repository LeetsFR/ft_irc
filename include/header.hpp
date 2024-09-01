#ifndef __HEADER__
#define __HEADER__

//C++
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

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

// COLOR
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

// MESSAGE
// MESSAGE: config server 
#define REP_CAPLS(serverName) (":" + serverName + " CAP * LS :multi-prefix\r\n")
#define REP_CAPREQ(serverName) (":" + serverName + " CAP * ACK :multi-prefix\r\n")
#define REP_CAPEND(clientName) (":" + clientName + " 001 " + clientName + " :Welcome to the Internet Relay Network " + clientName + ", my bros" + "\r\n")
#define REP_PONG(serverName) (":PONG " + serverName + "\r\n")

// MESSAGE: error message
#define ERR_NEEDMOREPARAMS(clientName, command) (":" + clientName + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_PASSWDMISMATCH(clientName) (":" + clientName + " 464 :Password incorrect password\r\n")
#define ERR_NOORIGIN(serverName, clientName) (":" + serverName + " 409 " + clientName + " :No origin specified\r\n")
#define ERR_NONICKNAMEGIVEN(clientName) (":" + clientName + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(clientName, nick) (":" + clientName + " 432 " + nick + " :Erroneus nickname\r\n")

//  Utils
std::vector<std::string> ft_split(const std::string&, const std::string&);

using namespace std;


#endif
