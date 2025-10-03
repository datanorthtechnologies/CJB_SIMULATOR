#ifndef CJBFSMDIDO_H
#define CJBFSMDIDO_H

#include <iostream>
#include "defines.h"

using namespace std;
class cjbfsmdido
{
public:
    cjbfsmdido();
    string user_in;
    void printMenu();
    void accessMenu();
    void handleUartTransmit(const string& user_in);
    void handleUartReceive(const string& user_in);
    void handleDIStatus();
};

#endif // CJBFSMDIDO_H
