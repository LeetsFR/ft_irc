#ifndef __LIBIRC__
#define __LIBIRC__

#include <arpa/inet.h>
#include <cctype>
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
#include <numeric>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#define RECV_SIZE 1024

typedef enum {
  ERROR = -1,
  IGNORE,

  PRIVMSG,
  PING,
  JOIN,

  KICK,
  INVITE,
  TOPIC,
  MODE_I,
  MODE_T,
  MODE_K,
  MODE_O,
  MODE_L
} typeMsg;

#define RPL_NAMREPLY(client, channel, userlist) client + " 353 " + client + " = " + channel + " :" + userlist + "\r\n"

#define PRIVMSG(sender, target, message) ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n"

#define RPL_ENDOFNAMES(client, channel) client + " 366 " + client + " " + channel + " :End of /NAMES list\r\n"

#define RPL_NAMREPLY_AND_ENDOFNAMES(client, channel, userlist) RPL_NAMREPLY(client, channel, userlist) + RPL_ENDOFNAMES(client, channel)

#define RPL_NOTOPIC(clientName, channelName) (clientName + " 331 " + channelName + " :No topic is set\r\n")

#define RPL_TOPIC(client, channel, topic) client + " 332 " + channel + " :" + topic + "\r\n"

#define REP_CAPEND(clientName) (string(":") + clientName + " 001 " + clientName + " :Welcome to the Internet Relay Network " + clientName + "\r\n")

#define REP_PONG(serverName) (string("PONG ") + serverName + "\r\n")

#define ERR_NEEDMOREPARAMS(clientName, command) (string(":") + clientName + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_PASSWDMISMATCH(clientName) (string(":") + clientName + " 464 " + clientName + " :Password incorrect\r\n")

#define ERR_NOORIGIN(serverName, clientName) (string(":") + serverName + " 409 " + clientName + " :No origin specified\r\n")

#define ERR_NONICKNAMEGIVEN(clientName) (string(":") + clientName + " 431 :No nickname given\r\n")
#define ERR_NICKNAMEINUSE(clientName, nick) (string(":") + clientName + " 433 " + nick + " :Nickname is already in use\r\n")
#define ERR_ERRONEUSNICKNAME(clientName, nick) (string(":") + clientName + " 432 " + nick + " :Erroneous nickname\r\n")

#define ERR_CANNOTSENDTOCHAN(clientName, channel) (string(":") + clientName + " 404 " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTEXTTOSEND(clientName) (string(":") + clientName + " 412 " + clientName + " :No text to send\r\n")
#define ERR_INPUTTOOLONG(clientName) (string(":") + clientName + " 417 " + clientName + " :Input line was too long\r\n")
#define ERR_NOSUCHCHANNEL(clientName, channel) (string(":") + clientName + " 403 " + channel + " :No such channel\r\n")
#define ERR_TOOMANYCHANNELS(clientName, channel) (string(":") + clientName + " 405 " + channel + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(clientName, channel) (string(":") + clientName + " 475 " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_BANNEDFROMCHAN(clientName, channel) (string(":") + clientName + " 474 " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_CHANNELISFULL(clientName, channel) (string(":") + clientName + " 471 " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(clientName, channel) (string(":") + clientName + " 473 " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(clientName, channel) (string(":") + clientName + " 476 " + channel + " :Bad channel mask\r\n")
#define ERR_CHANOPRIVSNEEDED(clientName, channel) (string(":") + clientName + " 482 " + channel + " :You're not channel operator\r\n")
#define ERR_USERNOTINCHANNEL(clientName, nick, channel) (string(":") + clientName + " 441 " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(clientName, channel) (string(":") + clientName + " 442 " + channel + " :You're not on that channel\r\n")
#define ERR_USERONCHANNEL(clientName, nick, channel) (string(":") + clientName + " 443 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(clientName, nick) (string(":") + clientName + " 401 " + nick + " :No such nick\r\n")

#define RPL_INVITING(client_nick, target_nick, channel_name)                                                                                                                                           \
  (std::string(":") + client_nick + " 341 " + target_nick + " " + channel_name + " :You have invited " + target_nick + " to " + channel_name + "\r\n")

using namespace std;

void handleSigint(int);
bool getMessage(int fd, string &message);
bool portIsValid(int port);
int convertIntSafe(const string &n);
string printTime();
vector<string> ft_split(const string &, const string &);
bool kickParsing(const string &message, string &, string &, string &);
bool joinParsing(string &message, vector<string> &channel, vector<string> &password);
bool inviteParsing(string &message, string &, string &);
void privmsgParsing(const string &message, string &, string &);
bool topicParsing(string &message, string &channelName, string &topicName);
bool modeParamParsing(string &message, string &channelName, string &mode, string &param);
bool modeParsing(string &message, string &channelName, string &mode);
void sendRC(const string &message, int socket);

#endif
