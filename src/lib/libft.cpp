#include "libft.hpp"

int convertIntSafe(string n) {
  int safe = 0;
  try {
    safe = stoi(n);
  } catch (const invalid_argument &ex) {
    cerr << "Error: invalid argument: " << ex.what() << endl;
  } catch (const out_of_range &ex) {
    cerr << "Error: out of range argument: " << ex.what() << endl;
  }
  return safe;
}
