#ifndef __LIBIRC__
#define __LIBIRC__

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <locale>
#include <csignal>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define RECV_SIZE 1024

using namespace std;

void handleSigint(int);
bool getMessage(int fd, string& message);
bool portIsValid(int port);
int convertIntSafe(const string& n);
string printTime();

extern bool quit;

#endif
