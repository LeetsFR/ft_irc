#include "IRC.hpp"
#include "libirc.hpp"

int main(int ac, char *av[]) {
  if (ac == 3)
  {
    try {
      IRC server(av[1], av[2]);
    } catch (logic_error &le) {
      cout << printTime() << RED << le.what() << RESET << endl;
    }
  }
  else
  {
    std::cout << "Usage: ./ircserv <PORT> <PASSWORD>\n";
    std::cout << "PORT options:\n";
    std::cout << "  - Standard ports: 194, 6665–6669, 6697\n";
    std::cout << "PASSWORD:\n";
    std::cout << "  - The password required for clients to connect to the server.\n";
  }
}
