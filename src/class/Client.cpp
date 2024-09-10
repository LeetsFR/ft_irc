/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 19:49:17 by scely             #+#    #+#             */
/*   Updated: 2024/09/10 13:09:01 by scely            ###   ########.fr       */
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
  if (this->_isConnected == false)
    this->configMessage(message, server);
  this->receiveMessage(message, server);
}

void Client::configMessage(std::string &message, IRC &server) {
  // faire une fonction dans le cas ou l'on recoit un message partiel
  // faire une fonction checker dans le cas ou lon recoit plusieurs fois la meme commande
  this->_messageTmp = ft_split(message, "\r\n");
  if (this->_messageTmp[0].compare("CAP LS") == 0)
    this->_messageTmp.erase(this->_messageTmp.begin());

  std::string currentMessage = this->_messageTmp[0];
  if (this->_isValidate == false && !currentMessage.compare(0, 4, "PASS")) {
    cout << "\n***********PASS***********\n";
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
    this->_messageTmp.erase(this->_messageTmp.begin());
    currentMessage = this->_messageTmp[0];
  }

  if (this->_badConfig == false && !currentMessage.compare(0, 4, "NICK") &&
      this->_nickname.size() == 0) {
    cout << "\n***********NICK***********\n";
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
      // RFC 1459 ils disent 9 caratere dans la 2812 aucune mention
      std::string wrongChar = ERR_ERRONEUSNICKNAME(this->_nickname, tmp);
      if (send(this->_socket, wrongChar.c_str(), wrongChar.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else if (server.findNickname(tmp) == true) {
      std::string alreadyUse = ERR_NICKNAMEINUSE(this->_nickname, tmp);
      if (send(this->_socket, alreadyUse.c_str(), alreadyUse.size(), 0) == -1)
        cerr << RED "Error: fail to send message" RESET << endl;
      this->_badConfig = true;
    } else {
      const_cast<std::string &>(this->_uniqId) = tmp;
      this->_nickname = tmp.substr(0, 9);
    }
    this->_messageTmp.erase(this->_messageTmp.begin());
    currentMessage = this->_messageTmp[0];
  }

  if (this->_badConfig == false && this->_nickname.size() &&
      !currentMessage.compare(0, 4, "USER")) {
    cout << "\n***********USER***********\n";
    std::vector<string> userParam = ft_split(currentMessage, " ");
    if (userParam.size() < 5) {
      // NEEDMOREPARAM
      this->_badConfig = true;
    }
    this->_username = "en attente de parsing";
    this->_messageTmp.erase(this->_messageTmp.begin());
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

int Client::receiveMessage(std::string &message, IRC &server) {
  
  typeMsg type = ERROR;
  if (message.find("PRIVMSG", 0))
  {
    //  ERR_NOSUCHNICK (401)
    //  ERR_CANNOTSENDTOCHAN (404)
    //  ERR_NOTEXTTOSEND (412) 
    //  ERR_INPUTTOOLONG (417)
    
  } else if (message.find("PING", 0))
  {
    //  ERR_NEEDMOREPARAMS (461) 
  } else if (message.find("JOIN", 0))
  {    
    // ERR_NEEDMOREPARAMS (461)
    // ERR_NOSUCHCHANNEL (403)
    // ERR_TOOMANYCHANNELS (405)
    // ERR_BADCHANNELKEY (475)
    // ERR_BANNEDFROMCHAN (474)
    // ERR_CHANNELISFULL (471)
    // ERR_INVITEONLYCHAN (473)
    // ERR_BADCHANMASK (476)

    // A Maxime de gerer
    // RPL_TOPIC (332)
    // RPL_TOPICWHOTIME (333)
    // RPL_NAMREPLY (353)
    // RPL_ENDOFNAMES (366)

  } else if (message.find("KICK", 0))
  {
    // ERR_NEEDMOREPARAMS (461)
    // ERR_NOSUCHCHANNEL (403)
    // ERR_CHANOPRIVSNEEDED (482)
    // ERR_USERNOTINCHANNEL (441)
    // ERR_NOTONCHANNEL (442)

  } else if (message.find("INVITE", 0))
  {

    // ERR_NEEDMOREPARAMS (461)
    // ERR_NOSUCHCHANNEL (403)
    // ERR_NOTONCHANNEL (442)
    // ERR_CHANOPRIVSNEEDED (482)
    // ERR_USERONCHANNEL (443)
    
    // a max de gerer
    // RPL_INVITING (341)

  } else if (message.find("TOPIC", 0))
  {
    // ERR_NEEDMOREPARAMS (461)
    // ERR_NOSUCHCHANNEL (403)
    // ERR_NOTONCHANNEL (442)
    // ERR_CHANOPRIVSNEEDED (482)

    // a max de gerer
    // RPL_NOTOPIC (331)
    // RPL_TOPIC (332)
    // RPL_TOPICWHOTIME (333)

  } 
  // =============================================
  // VOIR SI ON PEUT FAIRE UNE FONCTION GERER MODE
  if (message.find("MODE_I", 0))
  {
  } else if (message.find("MODE_T", 0))
  {
  } else if (message.find("MODE_K", 0))
  {
  } else if (message.find("MODE_O", 0))
  {
  } else if (message.find("MODE_L", 0))
  {

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

void Client::changeNickname(std::string &nick) {
  if (nick.size() > 9) {
    // send a error
    return;
  }
  this->_nickname = nick;
}
