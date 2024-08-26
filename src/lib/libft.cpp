#include "libft.hpp"

int convertIntSafe(string n) {
  int safe = 0;
  try {
    safe = stoi(n);
    return safe;
  }
  catch()
}
