//
// Created by Taka on 2019/11/18.
//

#include "multiuser.h"
#include <iostream>

void User::Init(string IP1, int ID1, int port1) {
    ID = ID1;
    port = port1;
    IP = move(IP1);
    name = "(no name)";
    pid = -1;
}
void User::Init(string IP1, int ID1, int port1, pid_t pid1) {
    ID = ID1;
    port = port1;
    IP = move(IP1);
    name = "(no name)";
    pid = pid1;
}
void User::Delete() {
    ID = -1;
    pid = -1;
    name = "(no name)";
}
User& User::operator=(const User &user) {
    ID = user.ID;
    port = user.port;
    IP = user.IP;
    name = user.name;
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
bool duplicateUser(const string &name, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID != -1){
            if (name == users[i].name){
                return true;
            }
        }
    }
    return false;
}
