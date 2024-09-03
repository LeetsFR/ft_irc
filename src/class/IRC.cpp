#include "IRC.hpp"
#include "Client.hpp"

IRC::IRC(const string &port, const string &password) : _name("Sunshine") {

  if (port.size() < 3 && port.size() > 4)
    throw logic_error("Exception: Port must be: 194 or [6665-6669] or 6697");
  char *endstr;
  double tmp = strtod(port.c_str(), &endstr);
  if (*endstr != '\0')
    throw logic_error("Exception: Port must be: 194 or [6665-6669] or 6697");
  if (tmp != 194 && (tmp < 6665 && tmp > 6669) && tmp != 6697)
    throw logic_error("Exception: Port must be: 194 or [6665-6669] or 6697");
  this->_port = tmp;
  this->_password = password;
  this->initSocketAdrr();
  this->initSocket();
}

IRC::~IRC() { close(this->_fdSocket); }

void IRC::initSocketAdrr() {
  this->_addr.sin_family = AF_INET; // IPV4
  this->_addr.sin_port = htons(this->_port);
  this->_addr.sin_addr.s_addr = htonl(0x7f000001); // 127.0.0.1 localhost
}

void IRC::initSocket() {
  int opt = 1; // ==> approondir aussi
  this->_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->_fdSocket == -1)
    throw logic_error("Exception: Cannot create socket");
  cout << GREEN "Success: Socket is created\n" RESET;

  if (setsockopt(this->_fdSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)))
    throw logic_error("Exception: Cannot bind the server to the address");
  cout << GREEN "Success: Socket is set\n" RESET;

  if (bind(this->_fdSocket, (struct sockaddr *)&this->_addr,
           sizeof(this->_addr)))
    throw logic_error("Exception: Cannot bind the server to the address");
  cout << GREEN "Success: Socket is bind to the address\n" RESET;

  if (listen(this->_fdSocket, 3)) // approfondir la notion
    throw logic_error("Error: Cannot listen to the port");
  std::cout << GREEN "Success: Socket is listening\n" RESET;
  fcntl(this->_fdSocket, F_SETFL, O_NONBLOCK);
}

int IRC::getSocket() { return this->_fdSocket; }

std::string IRC::getName() { return this->_name; }

void IRC::addClient(int clientSocket) {
  Client* newClient = new Client(clientSocket);
  cout << newClient << endl;
  this->_listClient.push_back(newClient);
  cout << this->_listClient[0] << endl;
  cout << "New client correctely added" << endl;
}

bool IRC::checkPassword(std::string& pass)
{ 
  return (pass == this->_password);
}

bool IRC::nicknameExist(std::string& nickname)
{
  (void)nickname;
  // pour le moment renvoi true
  return (true);
}

//tester avec les iterator pour voir si cest plus rapide
Client* IRC::findClient(int clientSocket)
{
    for (size_t i = 0; i < this->_listClient.size(); i++)
    {
      if ( this->_listClient[i]->getSocket() == clientSocket)
        return (this->_listClient[i]);
    }
    throw logic_error("Exception: tu codes comme une merde");
    return (NULL);
}

//proteger les fonctions
void IRC::deleteClient(int clientSocket)
{
    size_t i = 0;
    size_t nbClient = this->_listClient.size();    
    for (; i < nbClient; i++)
    {
        if (this->_listClient[i]->getSocket() == clientSocket)
        {
            cout << this->_listClient[i] << endl;
            cout << i << endl;
            delete this->_listClient[i];
            this->_listClient.erase(this->_listClient.begin() + i);
            close(clientSocket);
        }
    }
    cout << "Client "<< clientSocket << " has been removed\n";
}

void IRC::deleteAll()
{
    size_t i = 0;
    size_t nbClient = this->_listClient.size();    
    for (; i < nbClient; i++)
    {
      cout << this->_listClient[i] << endl;
      cout << i << endl;
      close(this->_listClient[i]->getSocket());
      delete this->_listClient[i];
      this->_listClient.erase(this->_listClient.begin() + i);
    }
    cout << "All client has been removed\n";
}
