#ifndef __CLIENT__
#define __CLIENT__

#include "header.hpp"

#define MAX_CLIENT_NAME 9

class IRC;

class Client {

    public:
        Client(int);
        ~Client();

        void        configMessage(std::string&, IRC& server);
        void        receiveMessage(std::string&, IRC& server);
        int         getSocket();
        std::string getNickname();
        void        changeNickname(std::string&);

    private:
        int          _socket;
        string       _nickname; // uniaue et fait 9 carateres
        const string _hostname; // le nom de la machine hote
        const string _username; // le nom du user sur la mahcine hote

        bool            _isValidate;
        bool            _isConnected;
        vector<string>  _messageTmp;
};

Client::Client(int socket) : _socket(socket), _isConnected(false), _isValidate(false) {}

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

// peut etre find sera mieux 

/*
pour les macro je peux directement les mettre dans la fonction send 
mais pour plus de lisibilite je prefere le mettre dans une string
*/
void Client::configMessage(std::string &message, IRC& server)
{
    this->_messageTmp = ft_split(message, "\r\n");
    if (!this->_messageTmp[0].compare("CAP LS"))
    {
        const string capLsMessage = REP_CAPLS(server.getName());
        if (send(this->_socket, capLsMessage.c_str(), capLsMessage.size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
    }

    if (this->_messageTmp[0].find("PASS", 0))
    {
        //TEST avec nc d'envoye "PASS" "PASS " "PASS 123" "PASS123"
        std::string pass = this->_messageTmp[0].substr(6);
        if (pass.size() == 0)
        {
            std::string needMore = ERR_NEEDMOREPARAMS(this->getNickname(), "PASS");
            if (send(this->_socket, needMore.c_str(), needMore.size(), 0) == -1)
                cerr << RED "Error: fail to send message" << endl;
        }
        this->_isValidate = server.checkPassword(pass);
        if (!this->_isValidate)
        {
            std::string wrongPassMessage = ERR_PASSWDMISMATCH(this->_nickname);
            if (send(this->_socket, wrongPassMessage.c_str(), wrongPassMessage.size(), 0) == -1)
               cerr << RED "Error: fail to send message" << endl;
        }
        this->_messageTmp.erase(this->_messageTmp.begin());
    }
    if (this->_messageTmp[0].find("NICK"))
    {
        this->_nickname = this->_messageTmp[0].substr(6);
        //  check le name avec les reste des name dans le server
        //  check si les caracteres sont bon
        //  il y a assez de paramettre donne
        this->_messageTmp.erase(this->_messageTmp.begin());

    }
    if (this->_messageTmp[0].find("USER"))
    {
        std::vector<string> userParam = ft_split(this->_messageTmp[0], " ");
        if (userParam.size() < 5)
        {
            // NEEDMOREPARAM
        }

        //  check si les caracteres sont bon
        //  il y a assez de paramettre donne
        this->_messageTmp.erase(this->_messageTmp.begin());
    }

    if (!this->_messageTmp[0].compare("CAP REQ :multi-prefix"))
    {
        std::string capReqMessage = REP_CAPREQ(server.getName());
        if (send(this->_socket, capReqMessage.c_str(), capReqMessage.size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        return ;
    }
    if (!this->_messageTmp[0].compare("CAP END"))
    {
        // cheker si tout les paramettres sont valdide
        std::string capEndMessage = REP_CAPEND(server.getName(), this->_nickname);
        if (send(this->_socket, capEndMessage.c_str(), capEndMessage.size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        this->_isConnected = true;
    }
    if (this->_messageTmp.size())
        cerr << RED "Error: to connect" << std::endl;
    // s'il y reste des chose dans message tmp,
    // cest que les message n'ont pas ete envoyer dans le bon ordre;
    /*
        CAP LS
        PASS
        NICK
        USER
        CAP REQ
        CAP END
    */
}

void    Client::receiveMessage(std::string& message, IRC& server)
{
    if (!message.find("PING", 0, 4))
    {
        string pingMessage = "PING " + server.getName() + "\r\n";
        if (message.compare(pingMessage))
        {
            std::string pongMessage = REP_PONG(server.getName());
            if (send(this->_socket, pongMessage.c_str(), pongMessage.size(), 0) == -1)
                cerr << RED "Error: fail to send message" << endl;
        }
        else
        {
            if (message.compare("PING\r\n"))
            {
                std::string noOriginMessage = ERR_NOORIGIN(server.getName(), this->_nickname);
                if (send(this->_socket, noOriginMessage.c_str(), noOriginMessage.size(), 0) == -1)
                    cerr << RED "Error: fail to send message" << endl;
            }
            else
            {
                std::string needMoreMessage = ERR_NEEDMOREPARAMS(server.getName(), this->_nickname);
                if (send(this->_socket, needMoreMessage.c_str(), needMoreMessage.size(), 0) == -1)
                    cerr << RED "Error: fail to send message" << endl;
            }


        }   
    // reste des commande;
    }
}






#endif
