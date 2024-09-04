/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 19:49:17 by scely             #+#    #+#             */
/*   Updated: 2024/09/04 22:24:14 by scely            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "Client.hpp"

Client::Client(int socket) : _socket(socket)
{
    this->_badConfig = false;
    this->_isConnected = false;
    this->_isValidate = false;
} 

Client::~Client() {}

int Client::getSocket() {return (this->_socket);}
std::string Client::getNickname() {return (this->_nickname);}
std::string Client::getUniqId() {return (this->_uniqId);}

void Client::changeNickname(std::string &nick)
{
    if (nick.size() > 9)
    {
        //send a error
        return ;
    }
    this->_nickname = nick;
}

void Client::handleMessage(std::string message, IRC& server)
{
    if (this->_isValidate == false && this->_isConnected == false)
        this->configMessage(message, server);
    this->receiveMessage(message, server);
}

void Client::configMessage(std::string& message, IRC& server)
{
    // faire une fonction dans le cas ou l'on recoit un message partiel
    // faire une fonction checker dans le cas ou lon recoit plusieurs fois la meme commande
    this->_messageTmp = ft_split(message, "\r\n");
    if (this->_messageTmp[0].compare("CAP LS") == 0)
        this->_messageTmp.erase(this->_messageTmp.begin());
    
    std::string currentMessage = this->_messageTmp[0];
    if (this->_isValidate == false && !currentMessage.compare(0, 4, "PASS"))
    {
        cout << "\n***********PASS***********\n";
        std::string tmp;
        if (currentMessage.find(' ', 4) != std::string::npos)
            tmp = currentMessage.substr(5);
        else
            tmp = "";
        if (tmp.size() == 0)
        {
            std::string needMore = ERR_NEEDMOREPARAMS(this->_nickname, "PASS");
            if (send(this->_socket, needMore.c_str(), needMore.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            this->_badConfig = true;
        }
        else if ((this->_isValidate = server.checkPassword(tmp)) == false)
        {
            std::string wrongPassMessage = ERR_PASSWDMISMATCH(this->_nickname);
            if (send(this->_socket, wrongPassMessage.c_str(), wrongPassMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            this->_badConfig = true;
        }
        this->_messageTmp.erase(this->_messageTmp.begin());
        currentMessage = this->_messageTmp[0];
    }

    if (this->_badConfig == false && !currentMessage.compare(0, 4, "NICK") && this->_nickname.size() == 0)
    {
        cout << "\n***********NICK***********\n";
        std::string tmp;
        if (currentMessage.find(' ', 4) != std::string::npos)
            tmp = currentMessage.substr(5);
        else
            tmp = "";
        if (tmp.size() == 0)
        {
            std::string noParam = ERR_NONICKNAMEGIVEN(this->_nickname);
            if (send(this->_socket, noParam.c_str(), noParam.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            this->_badConfig = true;
        }
        else if (this->correctNickFormat(tmp) == false)
        {
            // RFC 1459 ils disent 9 caratere dans la 2812 aucune mention
            std::string wrongChar = ERR_ERRONEUSNICKNAME(this->_nickname, tmp);
            if (send(this->_socket, wrongChar.c_str(), wrongChar.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            this->_badConfig = true;
        }
        else if (server.nicknameExist(tmp) == true)
        {
            std::string alreadyUse = ERR_NICKNAMEINUSE(this->_nickname, tmp);
            if (send(this->_socket, alreadyUse.c_str(), alreadyUse.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            this->_badConfig = true;
        }
        else 
        {
            const_cast<std::string&>(this->_uniqId) = tmp;
            this->_nickname = tmp;
        }
        this->_messageTmp.erase(this->_messageTmp.begin());
        currentMessage = this->_messageTmp[0];
    }
        
    if (this->_badConfig == false && this->_nickname.size() && !currentMessage.compare(0, 4, "USER"))
    {            
        cout << "\n***********USER***********\n";   
        std::vector<string> userParam = ft_split(currentMessage, " ");
        if (userParam.size() < 5)
        {
         // NEEDMOREPARAM
            this->_badConfig = true;
        }
        this->_messageTmp.erase(this->_messageTmp.begin());
    }


    /*================================================================*/
    if (this->_badConfig == false)
    {                          
        // cheker si tout les paramettres sont valdide
        // METTRE UN MESSAGE SPECIFIQUE SI LE PASS N'EST PAS SET
        std::string capEndMessage = REP_CAPEND( this->_nickname);
        if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
            cerr << RED "Error: fail to send message" RESET << endl;
        this->_isConnected = true;
    }
    else
    {
        std::string capEndMessage = "You cannot connect to the server\n";
        if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
            cerr << RED "Error: fail to send message" RESET << endl;
    }
}

void    Client::receiveMessage(std::string& message, IRC& server)
{
    if (!message.compare(0, 4, "PING"))
    {
        string pingMessage = "PING " + this->_nickname + "\r\n";
        if (!message.compare(pingMessage))
        {
            std::string pongMessage = REP_PONG(this->_nickname);
            if (send(this->_socket, pongMessage.c_str(), pongMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
        }
        else
        {
            if (!message.compare("PING\r\n"))
            {
                std::string needMoreMessage = ERR_NEEDMOREPARAMS(server.getName(), this->_nickname);
                if (send(this->_socket, needMoreMessage.c_str(), needMoreMessage.size(), 0) == -1)
                    cerr << RED "Error: fail to send message"  RESET << endl;
            }
            else
            {
                std::string noOriginMessage = ERR_NOORIGIN(server.getName(), this->_nickname);
                if (send(this->_socket, noOriginMessage.c_str(), noOriginMessage.size(), 0) == -1)
                    cerr << RED "Error: fail to send message" RESET << endl;
            }
        }   
    // reste des commande;
    }
}

bool    Client::correctNickFormat(std::string& nick)
{
    size_t nickSize = nick.size();
    if (std::isdigit(nick[0]))
        return (false);
        
    for (size_t i = 0; i < nickSize; i++)
    {
        if (!(std::isalnum(nick[i]) || (nick[i] >= '[' && nick[i] <= ']') || (nick[i] >= '{' && nick[i] <= '}')))
            return (false);
    }
    return (true);
}