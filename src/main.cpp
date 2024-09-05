#include "IRC.hpp"
#include <stdexcept>

int main() {
  try {
    IRC server("6667", "42");
  } catch (logic_error &le) {
    cout << le.what() << endl;
  }
}
