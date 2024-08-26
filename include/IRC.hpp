#ifndef __IRC__
#define __IRC__

#include "libft.hpp"

class IRC {

  public:
    IRC(const string &port, const string &password);
    ~IRC();

  private:
    int _fdsocket = 0;
    int _port = 0;
    string _password;

};

#endif
