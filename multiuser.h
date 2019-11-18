//
// Created by Taka on 2019/11/18.
//

#ifndef NETWORK_PROGRAMMING_MULTIUSER_H
#define NETWORK_PROGRAMMING_MULTIUSER_H

#include <string>
#include <map>
#include "command.h"

#define max_clients 30
using namespace std;

struct User{
    string name, IP;
    int ID, port;

    pid_t pid;

    void Init(string IP1, int ID1, int port1);
    void Init(string IP1, int ID1, int port1, pid_t pid1);
    void Delete();
    User& operator=(const User &user);
};
int addUser(const User &client, User users[]);
bool duplicateUser(const string &name, const User users[]);

#endif //NETWORK_PROGRAMMING_MULTIUSER_H
