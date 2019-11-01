#ifndef NETWORK_PROGRAMMING_USER_H
#define NETWORK_PROGRAMMING_USER_H

#endif //NETWORK_PROGRAMMING_USER_H

#include <string>
using namespace std;
struct User{
    string name, IP;
    int ID, port;
    static int n;
    void Init(string IP1, int ID1, int port1);
    void Delete();
};