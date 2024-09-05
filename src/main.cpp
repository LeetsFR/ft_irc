#include "IRC.hpp"
#include "libirc.hpp"

int main() {
  try {
    IRC server("6667", "42");
  } catch (logic_error &le) {
    cout << printTime() << RED << le.what() << RESET << endl;
  }
}
