#include "IRC.hpp"
#include "Poll.hpp"

IRC::IRC(const string &port, const string &password) {
  try {
    int tmp = stoi(port);
    if (tmp != 194 && (tmp < 6665 && tmp > 6669) && tmp != 6697)
      throw logic_error("Error: port must be: 194 or [6665-6669] or 6697");
    this->_port = tmp;
    this->_password = password;
    this->initSocketAdrr();
    this->initSocket();
  } catch (const invalid_argument &e) {
    cerr << "Error: invalid argument: " << e.what() << std::endl;
  } catch (const out_of_range &e) {
    cerr << "Error: out of range: " << e.what() << std::endl;
  }
  this->epoll = new Poll(this->_fdSocket);
}

IRC::~IRC() {
  close(this->_fdSocket);
  delete this->epoll;
}

void IRC::launchEpoll() { this->epoll->launchPoll(); }

void IRC::initSocketAdrr() {
  this->_addr.sin_family = AF_INET;
  this->_addr.sin_port = htons(this->_port);
  this->_addr.sin_addr.s_addr = htonl(0x7f000001);
}

void IRC::initSocket() {
  this->_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->_fdSocket == -1)
    throw logic_error("Error: Cannot create socket");
  cout << GREEN "Success: Socket is created\n" RESET;

  int opt = 1;
  if (setsockopt(this->_fdSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)))
    throw logic_error("Error: Cannot bind the server to the address");
  cout << GREEN "Success: Socket is set\n" RESET;

  if (bind(this->_fdSocket, (struct sockaddr *)&this->_addr,
           sizeof(this->_addr)))
    throw logic_error("Error: Cannot bind the server to the address");
  cout << GREEN "Success: Socket is bind to the address\n" RESET;
  fcntl(this->_fdSocket, F_SETFL, O_NONBLOCK);
  if (listen(this->_fdSocket, 3))
    throw logic_error("Error: Cannot listen to the port");
  std::cout << GREEN "Success: Socket is listening\n" RESET;
}

int IRC::getSocket() { return this->_fdSocket; }

void IRC::addClient(int client) {
  // Parsing a faire
  this->_tabClient.push_back(client);
  cout << "New client correctely added" << endl;
}
