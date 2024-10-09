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

// #define RPL_NAMREPLY(client, channel, userlist) client + " 353 " + client + " = " + channel + " :" + userlist + "\r\n"
// #define PRIVMSG(sender, target, message) ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n"
// #define RPL_ENDOFNAMES(client, channel) client + " 366 " + client + " " + channel + " :End of /NAMES list\r\n"
// #define RPL_NOTOPIC(clientNick, channelName) ("331 " + clientNick + " " + channelName + " :No topic is set\r\n")
// #define RPL_TOPIC(clientNick, channel, topic) ("332 " + clientNick + " " + channel + " :" + topic + "\r\n")
// #define RPL_NOTOPIC(clientNick, channelName) ("331 " + clientNick + " " + channelName + " :No topic is set\r\n")
// #define REP_CAPEND(clientName) (string(":") + clientName + " 001 " + clientName + " :Welcome to the Internet Relay Network " + clientName + "\r\n")
// #define REP_PONG(serverName) (string("PONG ") + serverName + "\r\n")
// #define ERR_NEEDMOREPARAMS(clientName, command) (string(":") + clientName + " 461 " + command + " :Not enough parameters\r\n")
// #define ERR_PASSWDMISMATCH(clientName) (string(":") + clientName + " 464 " + clientName + " :Password incorrect\r\n")
// #define ERR_NOORIGIN(serverName, clientName) (string(":") + serverName + " 409 " + clientName + " :No origin specified\r\n")
// #define ERR_NONICKNAMEGIVEN(clientName) (string(":") + clientName + " 431 :No nickname given\r\n")
// #define ERR_NICKNAMEINUSE(clientName, nick) (string(":") + clientName + " 432 " + nick + " :Nickname is already in use\r\n")
// #define ERR_ERRONEUSNICKNAME(clientName, nick) (string(":") + clientName + " 432 " + nick + " :Erroneous nickname\r\n")
// #define ERR_CANNOTSENDTOCHAN(clientName, channel) (string(":") + clientName + " 404 " + channel + " :Cannot send to channel\r\n")
// #define ERR_NOTEXTTOSEND(clientName) (string(":") + clientName + " 412 " + clientName + " :No text to send\r\n")
// #define ERR_INPUTTOOLONG(clientName) (string(":") + clientName + " 417 " + clientName + " :Input line was too long\r\n")
// #define ERR_NOSUCHCHANNEL(clientName, channel) (string(":") + clientName + " 403 " + channel + " :No such channel\r\n")
// #define ERR_TOOMANYCHANNELS(clientName, channel) (string(":") + clientName + " 405 " + channel + " :You have joined too many channels\r\n")
// #define ERR_BADCHANNELKEY(clientNick, channel) ("475 " + clientNick + " " + channel + " :Cannot join channel (+k)\r\n")
// #define ERR_BANNEDFROMCHAN(clientName, channel) (string(":") + clientName + " 474 " + channel + " :Cannot join channel (+b)\r\n")
// #define ERR_CHANNELISFULL(clientName, channel) (string(":") + clientName + " 471 " + channel + " :Cannot join channel (+l)\r\n")
// #define ERR_INVITEONLYCHAN(clientNick, channel) ("473 " + clientNick + " " + channel + " :Cannot join channel (+i)\r\n")
// #define ERR_BADCHANMASK(clientName, channel) (string(":") + clientName + " 476 " + channel + " :Bad channel mask\r\n")
// #define ERR_CHANOPRIVSNEEDED(clientName, channel) (string(":") + clientName + " 482 " + channel + " :You're not channel operator\r\n")
// #define ERR_USERNOTINCHANNEL(clientNick, nick, channel) ("441 " + clientNick + " " + nick + " " + channel + " :They aren't on that channel\r\n")
// #define ERR_NOTONCHANNEL(clientName, channel) (string(":") + clientName + " 442 " + channel + " :You're not on that channel\r\n")
// #define ERR_USERONCHANNEL(nick, channel) ("443" + nick + " " + channel + " :is already on channel\r\n")
// #define ERR_NOSUCHNICK(clientName, nick) (string(":") + clientName + " 401 " + nick + " :No such nick\r\n")
// #define RPL_INVITING(client_nick, target_nick, channel_name)
//(std::string(":") + client_nick + " 341 " + target_nick + " " + channel_name + " :You have invited " + target_nick + " to " + channel_name + "\r\n")

#define RPL_NAMREPLY(clientNick, channel, userlist) "353 " + clientNick + " = " + channel + " :" + userlist + "\r\n"
#define PRIVMSG(sender, target, message) ":" + sender + " PRIVMSG " + target + " :" + message + "\r\n"
#define RPL_ENDOFNAMES(clientNick, channel) "366 " + clientNick + " " + channel + " :End of /NAMES list\r\n"
#define RPL_NOTOPIC(clientNick, channelName) ("331 " + clientNick + " " + channelName + " :No topic is set\r\n")
#define RPL_TOPIC(clientNick, channel, topic) ("332 " + clientNick + " " + channel + " :" + topic + "\r\n")
#define REP_CAPEND(clientNick) "001 " + clientNick + " :Welcome to the Internet Relay Network " + clientNick + "\r\n"
#define REP_PONG(serverName) (string("PONG ") + serverName + "\r\n")
#define ERR_NEEDMOREPARAMS(clientNick, command) "461 " + clientNick + " " + command + " :Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH(clientNick) "464 " + clientNick + " :Password incorrect\r\n"
#define ERR_NOORIGIN(clientNick) "409 " + clientNick + " :No origin specified\r\n"
#define ERR_NONICKNAMEGIVEN(clientNick) "431 " + clientNick + " :No nickname given\r\n"
#define ERR_NICKNAMEINUSE(clientNick, nick) "433 " + clientNick + " " + nick + " :Nickname is already in use\r\n"
#define ERR_ERRONEUSNICKNAME(clientNick, nick) "432 " + clientNick + " " + nick + " :Erroneous nickname\r\n"
#define ERR_CANNOTSENDTOCHAN(clientNick, channel) "404 " + clientNick + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NOTEXTTOSEND(clientNick) "412 " + clientNick + " :No text to send\r\n"
#define ERR_INPUTTOOLONG(clientNick) "417 " + clientNick + " :Input line was too long\r\n"
#define ERR_NOSUCHCHANNEL(clientNick, channel) "403 " + clientNick + " " + channel + " :No such channel\r\n"
#define ERR_TOOMANYCHANNELS(clientNick, channel) "405 " + clientNick + " " + channel + " :You have joined too many channels\r\n"
#define ERR_BADCHANNELKEY(clientNick, channel) ("475 " + clientNick + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_BANNEDFROMCHAN(clientNick, channel) "474 " + clientNick + " " + channel + " :Cannot join channel (+b)\r\n"
#define ERR_CHANNELISFULL(clientNick, channel) "471 " + clientNick + " " + channel + " :Cannot join channel (+l)\r\n"
#define ERR_INVITEONLYCHAN(clientNick, channel) ("473 " + clientNick + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANMASK(clientNick, channel) "476 " + clientNick + " " + channel + " :Bad channel mask\r\n"
#define ERR_CHANOPRIVSNEEDED(clientNick, channel) "482 " + clientNick + " " + channel + " :You're not channel operator\r\n"
#define ERR_USERNOTINCHANNEL(clientNick, nick, channel) ("441 " + clientNick + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(clientNick, channel) "442 " + clientNick + " " + channel + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(nick, channel) ("443" + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(clientNick, nick) "401 " + clientNick + " " + nick + " :No such nick\r\n"
#define RPL_INVITING(clientNick, channelName, targetNick) "341 " + clientNick + " " + channelName + " " + targetNick + "\r\n"

using namespace std;

void handleSigint(int);
bool getMessage(int fd, string &message);
bool portIsValid(int port);
int convertIntSafe(const string &n);
string printTime();
vector<string> ft_split(const string &, const string &);
bool kickParsing(const string &message, string &, string &, string &);
bool joinParsing(string &message, string &channel, string &password);
bool inviteParsing(string &message, string &, string &);
void privmsgParsing(const string &message, string &, string &);
bool topicParsing(string &message, string &channelName, string &topicName);
bool modeParamParsing(string &message, string &channelName, string &mode, string &param);
bool modeParsing(string &message, string &channelName, string &mode);
void sendRC(const string &message, int socket);

#endif
