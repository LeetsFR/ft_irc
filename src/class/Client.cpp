/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: scely <scely@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 19:49:17 by scely             #+#    #+#             */
/*   Updated: 2024/09/02 20:53:10 by scely            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IRC.hpp"
#include "Client.hpp"

Client::Client(int socket) : _socket(socket), _isValidate(false), _isConnected(false) {} 

Client::~Client() {}

int Client::getSocket() {return (this->_socket);}
std::string Client::getNickname() {return (this->_nickname);}

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
    // faire une fonction dans le cas ou lon recois que le bon d'une commande
    this->_messageTmp = ft_split(message, "\r\n");
    size_t nbMessage = this->_messageTmp.size();
    while (nbMessage--) // a tester avec 0
    {
        std::string currentMessage = this->_messageTmp[0];
        if (!currentMessage.compare(0, 6, "CAP LS"))
        {
            const string capLsMessage = REP_CAPLS(server.getName());
            if (send(this->_socket, capLsMessage.c_str(), capLsMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;

        }
        else if (!currentMessage.compare(0, 4, "PASS"))
        {
            //TEST avec nc d'envoye "PASS" "PASS " "PASS 123" "PASS123"
            std::string tmp = currentMessage.substr(6);
            if (tmp.size() == 0)
            {
                std::string needMore = ERR_NEEDMOREPARAMS(this->getNickname(), "PASS");
                if (send(this->_socket, needMore.c_str(), needMore.size(), 0) == -1)
                    cerr << RED "Error: fail to send message" RESET << endl;
            }
            this->_isValidate = server.checkPassword(tmp);
            if (this->_isValidate)
            {
                std::string wrongPassMessage = ERR_PASSWDMISMATCH(this->_nickname);
                if (send(this->_socket, wrongPassMessage.c_str(), wrongPassMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            }
        }        
        else if (!currentMessage.find("NICK", 0))
        {
                              
            this->_nickname = currentMessage.substr(5);
            //  check le name avec les reste des name dans le server
            //  check si les caracteres sont bon
            //  il y a assez de paramettre donne
            // this->_messageTmp.erase(this->_messageTmp.begin());

        }
        else if (!currentMessage.find("USER"))
        {
                              
            std::vector<string> userParam = ft_split(currentMessage, " ");
            if (userParam.size() < 5)
            {
                // NEEDMOREPARAM
            }

            //  check si les caracteres sont bon
            //  il y a assez de paramettre donne
            // this->_messageTmp.erase(this->_messageTmp.begin());
        }
        else if (!currentMessage.compare("CAP REQ :multi-prefix"))
        {
            std::string capReqMessage = REP_CAPREQ(server.getName());
            if (send(this->_socket, capReqMessage.c_str(), capReqMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            // this->_messageTmp.erase(this->_messageTmp.begin());


        }
        else if (!currentMessage.compare("CAP END"))
        {
                              
            // cheker si tout les paramettres sont valdide
            std::string capEndMessage = REP_CAPEND( this->_nickname);
            if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" RESET << endl;
            // this->_messageTmp.erase(this->_messageTmp.begin());
            this->_isConnected = true;
            this->_isValidate = true;
        }
        this->_messageTmp.erase(this->_messageTmp.begin());
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