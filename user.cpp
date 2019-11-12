#include <iostream>
#include "user.h"
int User::n = 0;
void User::Init(string IP1, int ID1, int port1, int fd1) {
    ID = ID1;
    port = port1;
    IP = move(IP1);
    name = "(no name)";
    fd = fd1;
}
void User::Delete() {
    ID = -1;
    fd = -1;
    environment.clear();
    --n;
}
User& User::operator=(const User &user) {
    ID = user.ID;
    port = user.port;
    IP = user.IP;
    name = user.name;
    fd = user.fd;
}
int addUser(const User &client, User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID == -1 && users[i].fd == -1){
            users[i] = client;
            users[i].ID = i;
            ++User::n;
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
void User::Set() {
    map<string, string>::iterator it;
    for (it = environment.begin(); it !=  environment.end(); ++it) {
        setenv(it->first.c_str(), it->second.c_str(), true);
    }
}