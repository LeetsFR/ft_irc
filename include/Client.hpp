#ifndef __CLIENT__
#define __CLIENT__

#include "header.hpp"

#define MAX_CLIENT_NAME 9

class Client {

    public:
        Client(int);
        ~Client();

        void receiveMessage(std::string)

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

// peut etre find sera mieux 
Client::receiveMessage(std::string message)
{
    this->_messageTmp = message;
    if (!this->_messageTmp[0].compare("CAP LS\r\n"))
    {
        const string msg_1 = ":shining CAP * LS :multi-prefix\r\n";
        if (send(event[i].data.fd, msg_1.c_str(), msg_1.size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        // configure les reste name...
    }
    if (!this->_messageTmp[0].compare("CAP REQ :multi-prefix\r\n"))
    {
        const string msg_2 = ":shining CAP * ACK :multi-prefix\r\n";
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





#endif
