#ifndef NETWORK_PROGRAMMING_USER_H
#define NETWORK_PROGRAMMING_USER_H

#include <string>

#define max_clients 30

using namespace std;
struct User{
    string name, IP;
    int ID, port, fd;
    static int n;
    void Init(string IP1, int ID1, int port1, int fd1);
    void Delete();
    User& operator=(const User &user);
};
int addUser(const User &client, User users[]);
bool duplicateUser(const string &name, User users[]);
#endif //NETWORK_PROGRAMMING_USER_H


