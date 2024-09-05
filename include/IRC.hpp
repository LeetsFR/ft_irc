#ifndef __IRC__
#define __IRC__

#include "header.hpp"
class Client;

class IRC {

  public:
    IRC(const string &port, const string &password);
    ~IRC();

    std::string getName();
    bool        checkPassword(std::string&);
    int         getSocket();
    bool        nicknameExist(std::string&); // a faire... pour le moment renvoi true


    // void        addClient(int client);
    void        deleteClient(int clientSocket);
    Client*     findClient(int clientSocket);
    void        addClient(int);
    void        initSocketAdrr();
    void        initSocket();

  private:
    sockaddr_in   _addr;
    string        _name;
    string        _password;
    int           _port;
    int           _fdSocket;
    // vector<int>   _tabClient;
    vector<Client*>   _listClient;

};

#endif