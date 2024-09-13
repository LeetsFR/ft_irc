/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 19:49:17 by scely             #+#    #+#             */
/*   Updated: 2024/09/13 12:30:03 by scely            ###   ########.fr       */
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

/**************************************************************************************/
/*                                      Methodes                                      */
/**************************************************************************************/

int Client::getSocket() const { return (this->_socket); }

const string &Client::getIp() const { return (this->_ip); }

uint16_t Client::getPort() const { return ntohs(_clientAdress.sin_port); }

const string &Client::getNickname() const { return (this->_nickname); }

const string &Client::getUniqId() const { return (this->_uniqId); }

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
  // faire une fonction dans le cas ou l'on recoit un message partiel
  // faire une fonction checker dans le cas ou lon recoit plusieurs fois la meme commande
  // this->_messageTmp = ft_split(message, "\r\n");
  // if (this->_messageTmp[0].compare("CAP LS") == 0)
  //   this->_messageTmp.erase(this->_messageTmp.begin());

  std::string currentMessage = message;
  if (this->_isValidate == false && !currentMessage.compare(0, 4, "PASS")) {
    cout << "\n***********config PASS***********\n";
    std::string tmp;
    if (currentMessage.find(' ', 4) != std::string::npos)
      tmp = currentMessage.substr(5);
    else
      tmp = "";
    if (tmp.size() == 0) {
      std::string needMore = ERR_NEEDMOREPARAMS(this->_nickname, "PASS");
      if (send(this->_socket, needMore.c_str(), needMore.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else if ((this->_isValidate = server.checkPassword(tmp)) == false) {
      std::string wrongPassMessage = ERR_PASSWDMISMATCH(this->_nickname);
      if (send(this->_socket, wrongPassMessage.c_str(), wrongPassMessage.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    }
    // this->_messageTmp.erase(this->_messageTmp.begin());
    // currentMessage = this->_messageTmp[0];
  }

  if (this->_badConfig == false && !currentMessage.compare(0, 4, "NICK") &&
      this->_nickname.size() == 0) {
    cout << "\n***********config NICK***********\n";
    std::string tmp;
    if (currentMessage.find(' ', 4) != std::string::npos)
      tmp = currentMessage.substr(5);
    else
      tmp = "";
    if (tmp.size() == 0) {
      std::string noParam = ERR_NONICKNAMEGIVEN(this->_nickname);
      if (send(this->_socket, noParam.c_str(), noParam.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else if (this->correctNickFormat(tmp) == false) {
      std::string wrongChar = ERR_ERRONEUSNICKNAME(this->_nickname, tmp);
      if (send(this->_socket, wrongChar.c_str(), wrongChar.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else if (server.doesNicknameExist(tmp) == true) {
      std::string alreadyUse = ERR_NICKNAMEINUSE(this->_nickname, tmp);
      if (send(this->_socket, alreadyUse.c_str(), alreadyUse.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else {
      const_cast<std::string &>(this->_uniqId) = tmp;
      this->_nickname = tmp.substr(0, 9);
    }
    // this->_messageTmp.erase(this->_messageTmp.begin());
    // currentMessage = this->_messageTmp[0];
  }

  if (this->_badConfig == false && this->_nickname.size() &&
      !currentMessage.compare(0, 4, "USER")) {
    cout << "\n***********config USER***********\n";
    std::vector<string> userParam = ft_split(currentMessage, " ");
    if (userParam.size() < 5) {
      // NEEDMOREPARAM
      this->_badConfig = true;
    }
    std::string tmp; 
    if (currentMessage.find(':') != std::string::npos)
      tmp = currentMessage.substr(currentMessage.find(':') + 1);
    else
      tmp = "";
    if (tmp.size() == 0)
    {
      // NEEDMOREPARAM
      this->_badConfig = true;
    }
    this->_username = tmp;
    // this->_messageTmp.erase(this->_messageTmp.begin());
  }

  /*================================================================*/
  if (this->_isValidate == false || this->_badConfig == true) {
    std::string capEndMessage = "You cannot connect to the server\n";
    if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
  } else if (!this->_nickname.size() || !this->_username.size())
    return;
  else {
    std::string capEndMessage = REP_CAPEND(this->_nickname);
    if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    this->_isConnected = true;
  }
}

typeMsg Client::parsPrivmsg(string &message, IRC &server)
{
  cout << "\n***********cmd PRIVMSG***********\n";
  std::vector<std::string> privmsgParam = ft_split(message, " ");
  std::string errorMsg;
  if (privmsgParam.size() < 3)
  {
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "PRIVMSG");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);  
  }
  std::string currentToken;
  currentToken = privmsgParam[1];
  if (currentToken[0] == '#' && server.doesChannelExist(currentToken) == false)
  {
    errorMsg = ERR_NOSUCHCHANNEL(this->_nickname, currentToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  } else if (currentToken[0] != '#' && server.doesNicknameExist(currentToken) == false)
  {
    errorMsg = ERR_NOSUCHNICK(this->_nickname, currentToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  currentToken = privmsgParam[2];
  std::string text;
  if(currentToken[0] == ':')
    text = message.substr(message.find(":"));
  if (text.size() <= 1)
  {
    errorMsg = ERR_NOTEXTTOSEND(this->_nickname);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
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
    std::string errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "PING");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl; 
    return (ERROR); 
  }
  return (PING);
}

typeMsg Client::parsInvite(std::string &message, IRC &server)
{
  // [12:48:27] INVITE SAMUEL #SAMUEL
  // [12:48:48] INVITE SAMUEL SABRINA || error
  // [12:49:12] INVITE sabrina,carla #SAMUEL
  // [12:50:53] INVITE sabrina #SAMUEL
  // [12:51:00] INVITE  #SAMUEL || error

  // Gestion ==> max v
    // ERR_NOTONCHANNEL (442)
    // ERR_CHANOPRIVSNEEDED (482)
    // ERR_USERONCHANNEL (443)
  cout << "\n***********cmd INVITE***********\n";
  std::vector<std::string> inviteParam = ft_split(message, " ");
  std::string errorMsg;
  if (inviteParam.size() < 3)
  {
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "INVITE");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  } 
  else if (inviteParam.size() > 3)
  {
    // TOO MUCH PARAM  ????????
    // ERR_NEEDMOREPARAMS (461)
    return (ERROR);
  }
  std::string inviteToken = inviteParam[1];
  if (server.doesNicknameExist(inviteToken) == false)
  {
    errorMsg = ERR_NOSUCHNICK(this->_nickname, inviteToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR); 
  }
  inviteToken = inviteParam[2];
  if (server.doesChannelExist(inviteToken) == false)
  {
    errorMsg = ERR_NOSUCHCHANNEL(this->_nickname, inviteToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  return (INVITE);
}

typeMsg Client::parsTopic(std::string &message, IRC &server)
{
  // TOPIC #test :New topic          Setting the topic on "#test" to
  // TOPIC #test :                   ; Clearing the topic on "#test" => TOPIC -DELETE
  // TOPIC #test  

  // ERR_NOTONCHANNEL (442)
  // ERR_CHANOPRIVSNEEDED (482)
  cout << "\n***********cmd TOPIC***********\n";
  std::vector<std::string> topicParam = ft_split(message, " ");
  std::string errorMsg;
  if (topicParam.size() < 2)
  {
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "TOPIC");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  std::string topicToken = topicParam[1];
  if (server.doesChannelExist(topicToken) == false)
  {
    errorMsg = ERR_NOSUCHCHANNEL(this->_nickname, topicToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  return (TOPIC);
}

typeMsg Client::parsKick(std::string &message, IRC &server)
{
  // KICK #D SAMEUL : 
  // KICK #D KIKI :
  cout << "\n***********cmd KICK***********\n";
  std::vector<std::string> kickParam = ft_split(message, " ");
  std::string errorMsg;
  if (kickParam.size() < 4)
  {
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "KICK");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  std::string kickToken = kickParam[1];
  if (server.doesChannelExist(kickToken) == false)
  {
    errorMsg = ERR_NOSUCHCHANNEL(this->_nickname, kickToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  kickToken = kickParam[2];
  if (server.doesNicknameExist(kickToken) == false)
  {
    errorMsg = ERR_NOSUCHNICK(this->_nickname, kickToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  return (KICK);
}

typeMsg Client::parsMode(string &message, IRC &server)
{
  cout << "\n***********cmd MODE***********\n";
  std::vector<std::string> modeParam = ft_split(message, " ");
  std::string errorMsg;
  std::string modeToken;
  size_t nbParam = modeParam.size();
  
  modeToken = modeParam[1];
  if (server.doesChannelExist(modeToken) == false)
  {
    errorMsg = ERR_NOSUCHCHANNEL(this->_nickname, modeToken);
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  modeToken = modeParam[2];
  if (modeToken.size() > 2)
  {
    // too much param
    return (ERROR);
  } 
  else if (modeToken[0] != '-' && modeToken[0] != '+')
  {
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "MODE");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  else if (modeToken[1] == 'i')
    return (MODE_i);
  else if (modeToken[1] == 't')
    return (MODE_t);
  else if (modeToken[1] == 'k')
  {
    if (modeToken[0] == '-' || nbParam == 4)
      return (MODE_k);
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "MODE");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  else if (modeToken[1] == 'o')
  {
    if (nbParam == 4)
      return (MODE_o);
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "MODE");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  else if (modeToken[1] == 'l')
  {
    if (modeToken[0] == '-' || nbParam == 4)
      return (MODE_l);
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "MODE");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
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
    errorMsg = ERR_NEEDMOREPARAMS(this->_nickname, "JOIN");
    if (send(this->_socket, errorMsg.c_str(), errorMsg.size(), 0) == -1)
      cerr << RED "Error: fail to send message" RESET << endl;
    return (ERROR);
  }
  return (JOIN);
}

typeMsg Client::receiveMessage(std::string &message, IRC &server)
{
  if (message.find("PRIVMSG ", 0) != std::string::npos)
    return (this->parsPrivmsg(message, server));
  else if (message.find("PING ", 0) != std::string::npos)
    return (this->parsPing(message));
  else if (message.find("JOIN ", 0) != std::string::npos)
    return (this->parsJoin(message));
  else if (message.find("KICK ", 0) != std::string::npos)
    return (this->parsKick(message, server));
  else if (message.find("INVITE ", 0) != std::string::npos)
    return (this->parsInvite(message, server));
  else if (message.find("TOPIC ", 0) != std::string::npos)
    return (this->parsTopic(message, server));
  else if (message.find("MODE ", 0) != std::string::npos)
    return (this->parsMode(message, server));
  return (ERROR);
}
