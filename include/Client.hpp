#ifndef __CLIENT__
#define __CLIENT__

#include "header.hpp"
#include "IRC.hpp"
// class IRC;

#define MAX_CLIENT_NAME 9

class Client {

    public:
        Client(int);
        ~Client();

        void        handleMessage(std::string, IRC& server);
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
        const string _uniqId;

        bool            _isValidate;
        bool            _isConnected;
        bool            _badConfig;

        vector<string>  _messageTmp;
};


#endif
