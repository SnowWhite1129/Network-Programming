//
// Created by Taka on 2019/11/18.
//

#include "multiuser.h"
#include <iostream>
#include <cstring>

void User::Init(const char IP1[], int ID1, int port1, pid_t pid1) {
    ID = ID1;
    port = port1;
    strcpy(IP, IP1);
    strcpy(name, "(no name)");
    pid = pid1;
}
void User::Delete() {
    ID = -1;
    pid = -1;
    strcpy(name, "(no name)");
}
User& User::operator=(const User &user) {
    ID = user.ID;
    port = user.port;
    strcpy(IP, user.IP);
    strcpy(name, user.name);
    pid = user.pid;
}
int addUser(const User &client, User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID == -1){
            users[i] = client;
            users[i].ID = i;
            return i;
        }
    }
    return -1;
}
bool duplicateUser(const char name[], const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID != -1){
            if (strcmp(name, users[i].name)==0){
                return true;
            }
        }
    }
    return false;
}
