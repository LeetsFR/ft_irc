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
void Client::configMessage(std::string &message, IRC& server)
{
    this->_messageTmp = ft_split(message, "\r\n");
    if (!this->_messageTmp[0].compare("CAP LS"))
    {
        const string msg_1 = ":shining CAP * LS :multi-prefix\r\n";
        if (send(this->_socket, REP_CAPLS(server.getName()).c_str(), REP_CAPLS(server.getName()).size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        // configure les reste name...
        // configure les 
        this->_messageTmp.erase(this->_messageTmp.begin());

        if (this->_messageTmp[0].find("NICK"))
                this->_nickname = this->_messageTmp[0].substr(6);
        if (this->_messageTmp[1].find("PASS"))
        {
            std::string a = this->_messageTmp[1].substr(6);
            this->_isValidate = server.checkPassword(a);
            if (!this->_isValidate)
                if (send(this->_socket, ERR_PASSWDMISMATCH(this->_nickname).c_str(), ERR_PASSWDMISMATCH(this->_nickname).size(), 0) == -1)
                   cerr << RED "Error: fail to send message" << endl;
        }
        else
        {
            if (send(this->_socket, ERR_NEEDMOREPARAMS(this->getNickname(), "PASS").c_str(),ERR_NEEDMOREPARAMS(this->getNickname(), "PASS").size(), 0) == -1)
                cerr << RED "Error: fail to send message" << endl;
        }
        if (this->_messageTmp[2].find("USER"))
            //  COMPLETER
        return;
    }
    if (!this->_messageTmp[0].compare("CAP REQ :multi-prefix"))
    {
        // ":shining CAP * ACK :multi-prefix\r\n";
        if (send(this->_socket, REP_CAPREQ(server.getName()).c_str(), REP_CAPREQ(server.getName()).size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        return ;
    }
    if (!this->_messageTmp[0].compare("CAP END"))
    {
        if (send(this->_socket, REP_CAPEND(server.getName(), this->_nickname).c_str(), REP_CAPEND(server.getName(), this->_nickname).size(), 0) == -1)
            cerr << RED "Error: fail to send message" << endl;
        this->_messageTmp.erase(this->_messageTmp.begin());
        this->_isConnected = true;
        return ;
    }
}

void    Client::receiveMessage(std::string& message, IRC& server)
{
    if (!message.find("PING", 0, 4))
    {
        string pingMessage = "PING " + server.getName() + "\r\n";
        if (message.compare(pingMessage))
        {
            if (send(this->_socket, REP_PONG(server.getName()).c_str(), REP_PONG(server.getName()).size(), 0) == -1)
                cerr << RED "Error: fail to send message" << endl;
        }
        else
        {
            if (message.compare("PING\r\n"))
                if (send(this->_socket, ERR_NOORIGIN(server.getName(), this->_nickname).c_str(), ERR_NOORIGIN(server.getName(), this->_nickname).size(), 0) == -1)
                    cerr << RED "Error: fail to send message" << endl;
            else
                if (send(this->_socket, ERR_NEEDMOREPARAMS(server.getName(), this->_nickname).c_str(), ERR_NEEDMOREPARAMS(server.getName(), this->_nickname).size(), 0) == -1)
                    cerr << RED "Error: fail to send message" << endl;


        }   

            

    // p

    // reste des commande;
    }
}






#endif
