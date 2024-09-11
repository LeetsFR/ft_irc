#ifndef __LIBIRC__
#define __LIBIRC__

#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <locale>
#include <map>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#define RECV_SIZE 1024

#define REP_CAPLS(serverName) (string(":") + serverName + " CAP * LS :multi-prefix\r\n")
#define REP_CAPREQ(serverName) (string(":") + serverName + " CAP * ACK :multi-prefix\r\n")
#define REP_CAPEND(clientName) (string(":") + clientName + " 001 " + clientName + " :Welcome to the Internet Relay Network " + clientName + ", my bros\r\n")
#define REP_PONG(serverName) (string("PONG ") + serverName + "\r\n")

#define ERR_NEEDMOREPARAMS(clientName, command) (string(":") + clientName + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_PASSWDMISMATCH(clientName) (string(":") + clientName + " 464 :Password incorrect\r\n")
#define ERR_NOORIGIN(serverName, clientName) (string(":") + serverName + " 409 " + clientName + " :No origin specified\r\n")

#define ERR_NONICKNAMEGIVEN(clientName) (string(":") + clientName + " 431 :No nickname given\r\n")
#define ERR_NICKNAMEINUSE(clientName, nick) (string(":") + clientName + " 433 " + nick + " :Nickname is already in use\r\n")
#define ERR_ERRONEUSNICKNAME(clientName, nick) (string(":") + clientName + " 432 " + nick + " :Erroneous nickname\r\n")

using namespace std;

void handleSigint(int);
bool getMessage(int fd, string &message);
bool portIsValid(int port);
int convertIntSafe(const string &n);
string printTime();
vector<string> ft_split(const string &, const string &);
bool kickParsing(string &message, string &, string &, string &);
bool joinParsing(string &message, vector<string> &channel, vector<string> &code);

#endif
