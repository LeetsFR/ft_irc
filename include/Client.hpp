#ifndef __CLIENT__
#define __CLIENT__

#include "header.hpp"

#define MAX_CLIENT_NAME 9

class IRC server;

class Client {

    public:
        Client(int);
        ~Client();

        void        configMessage(std::string&);
        void        receiveMessage(std::string&);
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

Client::~Client();

int Client::getSocket() {return (this->_socket);}
int Client::getNickname() {return (this->_nickname);}

void Client::changeNickname(std::string &nick)
{
    if (nick.size() > 9)
    {
        //send a error
    }
    this->_nickname = nick;
}

// peut etre find sera mieux 
Client::configMessage(std::string &message)
{
    this->_messageTmp = message;
    if (!this->_messageTmp[0].compare("CAP LS\r\n"))
    {
        const string msg_1 = ":shining CAP * LS :multi-prefix\r\n";
        if (send(event[i].data.fd, msg_1.c_str(), msg_1.size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        // configure les reste name...
        // configure les 
        this->_messageTmp.erase(this->_messageTmp.begin());

        if (this->_messageTmp[0].find("NICK"))
                this->_nickname = this._messageTmp[0].substr(6);
        if (this->_messageTmp[1].find("PASS"))
            //  COMPARE AVEC LE SERVEUR
        if (this->_messageTmp[1].find("USER"))
            //  COMPLETER
        return;
    }
    if (!this->_messageTmp[0].compare("CAP REQ :multi-prefix\r\n"))
    {
        const string msg_2 = ":" + server.getName() + ":shining CAP * ACK :multi-prefix\r\n";
            if (send(event[i].data.fd, msg_2.c_str(), msg_2.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        return ;
    }
    if (!this->_messageTmp[0].compare("CAP END\r\n"))
    {
        const string msg_3 = ":shining 001 dan :Welcome to the Internet Relay Network dan\r\n";
            if (send(event[i].data.fd, msg_3.c_str(), msg_3.size(), 0) == -1)
              cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        this->_isConnected = true;
        return ;
    }
}

void    Client::receiveMessage(std::string& message)
{
    if (message.compare("PING\r\n"))
    // pong

    // reste des commande
}






#endif
