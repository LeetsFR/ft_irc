/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 19:49:17 by scely             #+#    #+#             */
/*   Updated: 2024/09/14 14:20:35 by scely            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "IRC.hpp"

/**************************************************************************************/
/*                              Constructeur et destructeur                           */
/**************************************************************************************/

Client::Client(int socket, sockaddr_in clientAdress, string ip)
    : _socket(socket), _clientAdress(clientAdress), _ip(ip) {
  this->_badConfig = false;
  this->_isConnected = false;
  this->_isValidate = false;
  this->_prevMsgIncomplete = false;
}

Client::~Client() {
  // close(this->_socket);
}

/**************************************************************************************/
/*                              Surcharge d'operateur                                 */
/**************************************************************************************/

bool Client::operator==(const Client &client) const { return (_uniqId == client.getUniqId()); }

std::ostream& operator<<(std::ostream& os, const Client& obj)
{
    os << "NICK: " << obj.getNickname() << endl;
    os << "USER: " << obj.getUser() << endl;
    return os;
}

/**************************************************************************************/
/*                                      Methodes                                      */
/**************************************************************************************/

int Client::getSocket() const { return (this->_socket); }

const string &Client::getIp() const { return (this->_ip); }

uint16_t Client::getPort() const { return ntohs(_clientAdress.sin_port); }

const string &Client::getNickname() const { return (this->_nickname); }

const string &Client::getHostname() const { return (this->_hostname); }

const string &Client::getUniqId() const { return (this->_uniqId); }

const string &Client::getUser() const { return (this->_username); }

void Client::handleMessage(std::string message, IRC &server) {

  if (this->_prevMsgIncomplete == true)
    message.insert(0, this->_messageTmp[0]);
  this->_messageTmp = ft_split(message, "\r\n");

  if (message.size() < 3 || message.find("\r\n", message.size() - 2) == std::string::npos)
    this->_prevMsgIncomplete = true;
  else
    this->_prevMsgIncomplete = false;
  int i = this->_messageTmp.size() - this->_prevMsgIncomplete; 
 
  while (i-- > 0)
  {
    if (this->_isConnected == false)
      this->configMessage(this->_messageTmp[0], server);
    else
      this->receiveMessage(this->_messageTmp[0], server);
    this->_messageTmp.erase(this->_messageTmp.begin());
  }
}

bool Client::correctNickFormat(std::string &nick) {
  size_t nickSize = nick.size();
  if (std::isdigit(nick[0]))
    return (false);

  for (size_t i = 0; i < nickSize; i++) {
    if (!(std::isalnum(nick[i]) || (nick[i] >= '[' && nick[i] <= ']') ||
          (nick[i] >= '{' && nick[i] <= '}')))
      return (false);
  }
  return (true);
}

void Client::configMessage(std::string &message, IRC &server)
{
  std::string tmp;
  if (!message.compare("CAP LS"))
    return ;
  if (this->_isValidate == false && !message.compare(0, 4, "PASS")) {
    cout << "\n***********config PASS***********\n";
    if (message.find(' ', 4) != std::string::npos)
      tmp = message.substr(5);
    else
      tmp = "";
    if (tmp.size() == 0) {
      sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "PASS"));
      this->_badConfig = true;
    } else if ((this->_isValidate = server.checkPassword(tmp)) == false) {
      sendMsgToClient(ERR_PASSWDMISMATCH(this->_nickname));
      this->_badConfig = true;
    }
  }

  if (this->_badConfig == false && !message.compare(0, 4, "NICK") &&
      this->_nickname.size() == 0) {
    cout << "\n***********config NICK***********\n";
    if (message.find(' ', 4) != std::string::npos)
      tmp = message.substr(5);
    else
      tmp = "";
    if (tmp.size() == 0) {
      sendMsgToClient(ERR_NONICKNAMEGIVEN(this->_nickname));
      this->_badConfig = true;
    } else if (this->correctNickFormat(tmp) == false) {
      sendMsgToClient(ERR_ERRONEUSNICKNAME(this->_nickname, tmp));
      this->_badConfig = true;
    } else if (server.doesNicknameExist(tmp) == true) {
      sendMsgToClient(ERR_NICKNAMEINUSE(this->_nickname, tmp));
      this->_badConfig = true;
    } else {
      const_cast<std::string &>(this->_uniqId) = tmp;
      this->_nickname = tmp.substr(0, 9);
    }
  }

  if (this->_badConfig == false && this->_nickname.size() &&
      !message.compare(0, 4, "USER")) {
    cout << "\n***********config USER***********\n";
    std::vector<string> userParam = ft_split(message, " ");
    if (userParam.size() < 5 || message.find(':') == std::string::npos) {
      sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "USER"));
      this->_badConfig = true;
    }
    tmp = message.substr(message.find(':') + 1);
    this->_username = tmp;
  }

  /*================================================================*/
  if (this->_isValidate == false || this->_badConfig == true)
    sendMsgToClient("You cannot connect to the server\n");
  else if (!this->_nickname.size() || !this->_username.size())
    return;
  else {
    sendMsgToClient(REP_CAPEND(this->_nickname));
    this->_isConnected = true;
  }
}

void Client::sendMsgToClient(const std::string& message)
{
  if (send(this->_socket, message.c_str(), message.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
  cout << message;
}

typeMsg Client::parsPrivmsg(string &message, IRC &server)
{
  cout << "\n***********cmd PRIVMSG***********\n";
  std::vector<std::string> privmsgParam = ft_split(message, " ");
  if (privmsgParam.size() < 3)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "PRIVMSG"));
    return (ERROR);  
  }
  std::string currentToken;
  currentToken = privmsgParam[1];
  if (currentToken[0] == '#' && server.doesChannelExist(currentToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHCHANNEL(this->_nickname, currentToken));
    return (ERROR);
  } else if (currentToken[0] != '#' && server.doesNicknameExist(currentToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHNICK(this->_nickname, currentToken));
    return (ERROR);
  }
  currentToken = privmsgParam[2];
  std::string text;
  if(currentToken[0] == ':')
    text = message.substr(message.find(":") + 1);
  size_t i = 0;
  while (i < text.size() && (std::isspace(text[i]) || std::iscntrl(text[i])))
    ++i;
  if (i == text.size())
  {
    sendMsgToClient(ERR_NOTEXTTOSEND(this->_nickname));
    return (ERROR);
  }
  return (PRIVMSG); 
}

typeMsg Client::parsPing(std::string &message)
{
  cout << "\n***********cmd PING***********\n";
  std::vector<std::string> pingParam = ft_split(message, " ");
  if (pingParam.size() < 2)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "PING"));
    return (ERROR); 
  }
  return (PING);
}

typeMsg Client::parsInvite(std::string &message, IRC &server)
{
  cout << "\n***********cmd INVITE***********\n";
  std::vector<std::string> inviteParam = ft_split(message, " ");
  if (inviteParam.size() < 3)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "INVITE"));
    return (ERROR);
  }
  std::string inviteToken = inviteParam[1];
  if (server.doesNicknameExist(inviteToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHNICK(this->_nickname, inviteToken));
    return (ERROR); 
  }
  inviteToken = inviteParam[2];
  if (server.doesChannelExist(inviteToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHCHANNEL(this->_nickname, inviteToken));
    return (ERROR);
  }
  return (INVITE);
}

typeMsg Client::parsTopic(std::string &message, IRC &server)
{
  cout << "\n***********cmd TOPIC***********\n";
  std::vector<std::string> topicParam = ft_split(message, " ");
  if (topicParam.size() < 2)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "TOPIC"));
    return (ERROR);
  }
  std::string topicToken = topicParam[1];
  if (server.doesChannelExist(topicToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHCHANNEL(this->_nickname, topicToken));
    return (ERROR);
  }
  return (TOPIC);
}

typeMsg Client::parsKick(std::string &message, IRC &server)
{
  cout << "\n***********cmd KICK***********\n";
  std::vector<std::string> kickParam = ft_split(message, " ");
  if (kickParam.size() < 3)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "KICK"));
    return (ERROR);
  }
  std::string kickToken = kickParam[1];
  if (server.doesChannelExist(kickToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHCHANNEL(this->_nickname, kickToken));
    return (ERROR);
  }
  kickToken = kickParam[2];
  if (server.doesNicknameExist(kickToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHNICK(this->_nickname, kickToken));
    return (ERROR);
  }
  return (KICK);
}

typeMsg Client::parsMode(string &message, IRC &server)
{
  cout << "\n***********cmd MODE***********\n";
  if (message == ("MODE " + this->_nickname + " +i"))
    return (IGNORE);
  std::vector<std::string> modeParam = ft_split(message, " ");
  std::string modeToken;
  size_t nbParam = modeParam.size();
  
  modeToken = modeParam[1];
  if (server.doesChannelExist(modeToken) == false)
  {
    sendMsgToClient(ERR_NOSUCHCHANNEL(this->_nickname, modeToken));
    return (ERROR);
  }
  modeToken = modeParam[2];
  if ((modeToken[0] != '-' && modeToken[0] != '+') || modeToken.size() != 2)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "MODE"));
  }
  else if (modeToken[1] == 'i')
    return (MODE_i);
  else if (modeToken[1] == 't')
    return (MODE_t);
  else if (modeToken[1] == 'k')
  {
    if (modeToken[0] == '-' || nbParam == 4)
      return (MODE_k);
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "MODE"));
  }
  else if (modeToken[1] == 'o')
  {
    if (nbParam == 4)
      return (MODE_o);
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "MODE"));
  }
  else if (modeToken[1] == 'l')
  {
    if (modeToken[0] == '-' || nbParam == 4)
      return (MODE_l);
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "MODE"));
  }
  return (ERROR);
}

typeMsg Client::parsJoin(string &message)
{
  cout << "\n***********cmd JOIN***********\n";
  std::vector<std::string> joinParam = ft_split(message, " ");
  std::string errorMsg;

  if (joinParam.size() < 2)
  {
    sendMsgToClient(ERR_NEEDMOREPARAMS(this->_nickname, "JOIN"));
    return (ERROR);
  }
  return (JOIN);
}

typeMsg Client::receiveMessage(std::string &message, IRC &server)
{
  std::vector<std::string> cmdToken = ft_split(message, " ");
  if (cmdToken.size() < 2)
    return (ERROR);
  else if (cmdToken[0].compare("PRIVMSG") == 0)
    return (this->parsPrivmsg(message, server));
  else if (cmdToken[0].compare("PING") == 0)
    return (this->parsPing(message));
  else if (cmdToken[0].compare("JOIN") == 0)
    return (this->parsJoin(message));
  else if (cmdToken[0].compare("KICK") == 0)
    return (this->parsKick(message, server));
  else if (cmdToken[0].compare("INVITE") == 0)
    return (this->parsInvite(message, server));
  else if (cmdToken[0].compare("TOPIC") == 0)
    return (this->parsTopic(message, server));
  else if (cmdToken[0].compare("MODE") == 0)
    return (this->parsMode(message, server));
  return (ERROR);
}
