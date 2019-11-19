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
    char name[200], IP[50];
    int ID, port;
    pid_t pid;

    void Init(const char IP1[], int ID1, int port1, pid_t pid1);
    void Delete();
    User& operator=(const User &user);
};
int addUser(const User &client, User users[]);
bool duplicateUser(const char name[], const User users[]);

#endif //NETWORK_PROGRAMMING_MULTIUSER_H
