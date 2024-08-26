#include "IRC.hpp"
#include <stdexcept>

int main(void) { 
  try{
  IRC server("6667", "42"); 
  server.launchEpoll();
  }
  catch(const logic_error& e)
  {
    cerr << e.what() << endl;
  }
}


