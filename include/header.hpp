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

// CLASS
// #include "Client.hpp"
// #include "Channel.hpp"
#include "IRC.hpp"

// COLOR
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

// MESSAGE
// MESSAGE: config server 
#define REP_CAPLS(serverName) (":" + serverName + " CAP * LS :multi-prefix\r\n")
#define REP_CAPREQ(serverName) (":" + serverName + " CAP * ACK :multi-prefix\r\n")
#define REP_CAPEND(serverName, clientName) (":" + serverName + " 001 " + clientName + " :Welcome to the Internet Relay Network " + clientName + "\r\n")
#define REP_PONG(serverName) (":" + serverName)

// MESSAGE: error message
#define ERR_NEEDMOREPARAMS(clientName, command) ("PONG:" + clientName + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_PASSWDMISMATCH(clientName) (":" + clientName + " 464 :Password incorrect password\r\n")
#define ERR_NOORIGIN(serverName, clientName) (":" + serverName + " 409 " + clientName + " :No origin specified\r\n")


using namespace std;

//  Utils
std::vector<std::string> ft_split(std::string str, std::string delim);


#endif
