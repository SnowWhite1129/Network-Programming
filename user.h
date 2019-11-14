#ifndef NETWORK_PROGRAMMING_USER_H
#define NETWORK_PROGRAMMING_USER_H

#include <string>
#include <map>
#include "command.h"

#define max_clients 30

using namespace std;
struct User{
    string name, IP;
    int ID, port;
    static int n;

    int fd;
    map<string, string> environment;
    command cmd[MAXLIST];

    pid_t pid;

    void Init(string IP1, int ID1, int port1, int fd1);
    void Init(string IP1, int ID1, int port1, int fd1, pid_t pid1);
    void Delete();
    void Set();
    User& operator=(const User &user);
};
int addUser(const User &client, User users[]);
bool duplicateUser(const string &name, const User users[]);

#endif //NETWORK_PROGRAMMING_USER_H


