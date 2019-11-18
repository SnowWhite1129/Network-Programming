//
// Created by Taka on 2019/11/18.
//

#include "multimessage.h"
#include <sys/types.h>
#include <signal.h>
#include "message.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "user.h"


void welcomeMessage(){
    cout << "****************************************\n"
            "** Welcome to the information server. **\n"
            "****************************************" << endl;
}
void loginMessage(const char IP[], int port, ShareMemory *shm, int senderID, int clientID){
    char buffer[1025];
    sprintf(buffer, "*** User '(no name)' entered from %s:%d. ***\n", IP, port);
    strcpy(shm->message[senderID][clientID], buffer);
}
void logoutMessage(const char name[], ShareMemory *shm, int senderID, int clientID){
    char buffer[1025];
    sprintf(buffer, "*** User '%s' left. ***\n", name);
    strcpy(shm->message[senderID][clientID], buffer);
}
void yellMessage(const char name[], const char message[], ShareMemory *shm, int senderID, int clientID){
    char buffer[1025];
    sprintf(buffer, "*** %s yelled ***: %s\n", name, message);
    strcpy(shm->message[senderID][clientID], buffer);
}
void toldMessage(const char name[], const char message[], ShareMemory *shm, int senderID, int clientID){
    char buffer[1025];
    sprintf(buffer, "*** %s told you ***: %s\n", name, message);
    strcpy(shm->message[senderID][clientID], buffer);
}
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID, ShareMemory *shm){
    char buffer[1025] = {0};
    sprintf(buffer, "*** %s (#%d) just piped '%s' to %s (#%d) ***\n",
            sendername, senderID+1, message, receivername, receiverID+1);
    strcpy(shm->message[senderID][receiverID], buffer);
}
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID, ShareMemory *shm){
    char buffer[1025];
    sprintf(buffer, "*** %s (#%d) just received from %s (#%d) by '%s' ***\n",
            receivername, receiverID+1, sendername, senderID+1, message);
    strcpy(shm->message[senderID][receiverID], buffer);
}
void nouserMessage(int ID){
    cout << "*** Error: user #" << ID << " does not exist yet. ***" << endl;
}
void nomessageMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " does not exist yet. ***" << endl;
}
void occuipiedMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " already exists. ***" << endl;
}
void duplicatNameMessage(const string& name){
    cout <<  "*** User '" << name << "' already exists. ***\n";
}
void nameMessage(int clientID, const char IP[], int port, const char name[], ShareMemory *shm){
    char buffer[1025];
    sprintf(buffer, "*** User from %s:%d is named '%s'. ***\n", IP, port, name);
    for (int i = 0; i < max_clients; ++i) {
        strcpy(shm->message[clientID][i], buffer);
        kill(shm->users[i].pid, SIGUSR2);
    }
}
void login(int newclient, ShareMemory *shm){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].ID!=-1){
            loginMessage(shm->users[newclient].IP.c_str(), shm->users[newclient].port, shm, newclient, i);
            kill(shm->users[i].pid, SIGUSR2);
        }
    }
}
void logout(int newclient, ShareMemory *shm){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].fd != -1){
            logoutMessage(shm->users[newclient].name.c_str(), shm, newclient, i);
            kill(shm->users[i].pid, SIGUSR2);
        }
    }
}
void recieve(int receiverID, int senderID, const string &message, ShareMemory *shm){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].fd!=-1) {
            receiveMessage(shm->users[receiverID].name.c_str(), receiverID, message.c_str(),
                           shm->users[senderID].name.c_str(), senderID, shm);
            kill(shm->users[i].pid, SIGUSR2);
        }
    }
}
void send(int senderID, int receiverID, const string &message, ShareMemory *shm){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].fd!=-1) {
            sendMessage(shm->users[senderID].name.c_str(), senderID, message.c_str(),
                        shm->users[receiverID].name.c_str(), receiverID, shm);
            kill(shm->users[i].pid, SIGUSR2);
        }
    }
}
void yell(int clientID, ShareMemory *shm, const char message[]){
    for (int i = 0; i < max_clients; ++i) {
        strcpy(shm->message[clientID][i], message);
        yellMessage(shm->users[clientID].name.c_str(), message, shm, clientID, i);
        kill(shm->users[i].pid, SIGUSR2);
    }
}
void tell(int sender, int receiver, const char message[], ShareMemory *shm){
    if (shm->users[sender].ID == -1){
        nouserMessage(sender);
    } else{
        string line = message;
        line = line.substr(line.find(' ')+1);
        line = line.substr(line.find(' ')+1);
        strcpy(shm->message[sender][receiver], line.c_str());
        toldMessage(shm->users[sender].name.c_str(), line.c_str(), shm, sender, receiver);
        kill(shm->users[receiver].pid, SIGUSR2);
    }
}
void userMessage(const User &user){
    cout << user.ID+1 << "    " << user.name.c_str() << "    " << user.IP.c_str() << "    " << user.port;
}
void whoMessage(int clientID, const User users[]){
    cout << "<ID>    <nickname>    <IP:port>    <indicate me>\n";
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID != -1)
            userMessage(users[i]);
        if (i == clientID)
            cout << "    <-me";
        if (users[i].ID != -1)
            cout << "\n";
    }
}
void who(int clientID, const User users[]){
    whoMessage(clientID, users);
}
void name(int clientID, const string &name, ShareMemory *shm){
    if (duplicateUser(name, shm->users)){
        duplicatNameMessage(name);
    } else{
        shm->users[clientID].name = name;
        nameMessage(clientID, shm->users[clientID].IP.c_str(), shm->users[clientID].port, name.c_str(), shm);
    }
}