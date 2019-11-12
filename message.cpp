#include "message.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include "user.h"
using namespace std;
void welcomeMessage(int fd){
    char buffer[1025];
    sprintf(buffer, "***************************************\n"
            "** Welcome to the information server. **\n"
            "***************************************\n");
    write(fd, buffer, strlen(buffer));
}
void loginMessage(const char IP[], int port, int fd){
    char buffer[1025];
    sprintf(buffer, "*** User '(no name)' entered from %s:%d. ***\n", IP, port);
    write(fd, buffer, strlen(buffer));
}
void logoutMessage(const char name[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** User '%s' left. ***\n", name);
    write(fd, buffer, strlen(buffer));
}
void yellMessage(const char name[], const char message[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** %s yelled ***: %s\n", name, message);
    write(fd, buffer, strlen(buffer));
}
void toldMessage(const char name[], const char message[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** %s told you ***: %s\n", name, message);
    write(fd, buffer, strlen(buffer));
}
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID, int fd){
    char buffer[1025] = {0};
    sprintf(buffer, "*** %s(#%d) just piped '%s' to %s (#%d) ***\n",
            sendername, senderID+1, message, receivername, receiverID+1);
    write(fd, buffer, strlen(buffer));
}
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** %s(#%d) just received from %s (#%d) by '%s' ***\n",
            receivername, receiverID+1, sendername, senderID+1, message);
    write(fd, buffer, strlen(buffer));
}
void nouserMessage(int ID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: user #%d does not exist yet. ***\n", ID);
    write(fd, buffer, strlen(buffer));
}
void nomessageMessage(int senderID, int receiverID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: the pipe #%d->#%d does not exist yet. ***\n", senderID+1, receiverID+1);
    write(fd, buffer, strlen(buffer));
}
void occuipiedMessage(int senderID, int receiverID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: the pipe #%d->#%d already exists. ***\n", senderID+1, receiverID+1);
    write(fd, buffer, strlen(buffer));
}
void nameMessage(const char IP[], int port, const char name[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** User from %s:%d is named '%s'. ***\n", IP, port, name);
    write(fd, buffer, strlen(buffer));
}
void duplicatNameMessage(const char name[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** User '%s' already exists. ***\n", name);
    write(fd, buffer, strlen(buffer));
}
void whoMessage(int clientID, const User users[], int fd){
    char buffer[1025];
    sprintf(buffer, "<ID>    <nickname>    <IP:port>    <indicate me>\n");
    write(fd, buffer, strlen(buffer));
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID != -1)
            userMessage(users[i], fd);
        if (i == clientID)
            write(fd, "<-me", strlen("<-me"));
        if (users[i].ID != -1)
            write(fd, "\n", strlen("\n"));
    }
}
void userMessage(const User &user, int fd){
    char buffer[1025];
    sprintf(buffer, "%d    %s     %s:%d    ", user.ID+1, user.name.c_str(), user.IP.c_str(), user.port);
    write(fd, buffer, strlen(buffer));
}
void login(int newclient, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].fd!=-1 && i != newclient)
            loginMessage(users[newclient].IP.c_str(), users[newclient].port, users[i].fd);
    }
}
void logout(int newclient, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].fd != -1){
            logoutMessage(users[newclient].name.c_str(), users[i].fd);
        }
    }
}
void recieve(int receiverID, int senderID, const string &message, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].fd!=-1){
            receiveMessage(users[receiverID].name.c_str(), receiverID, message.c_str(), users[senderID].name.c_str(), senderID, users[i].fd);
        }
    }
}
void send(int senderID, int receiverID, const string &message, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].fd!=-1){
            sendMessage(users[senderID].name.c_str(), senderID, message.c_str(), users[receiverID].name.c_str(), receiverID, users[i].fd);
        }
    }
}
void yell(int clientID, const string& message, const User users[]){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].ID != -1){
            yellMessage(users[clientID].name.c_str(), message.c_str(), users[i].fd);
        }
    }
}
void tell(int sender, int receiver, const string& message, const User users[]){
    if (users[sender].ID == -1){
        nouserMessage(sender, users[sender].fd);
    } else{
        toldMessage(users[sender].name.c_str(), message.c_str(), users[receiver].fd);
    }
}
void who(int clientID, const User users[]){
    whoMessage(clientID, users, users[clientID].fd);
}
void name(int clientID, const string &name, User users[]){
    if (duplicateUser(name, users)){
        duplicatNameMessage(name.c_str(), users[clientID].fd);
    } else{
        for (int i = 0; i < max_clients; ++i) {
            if (users[i].ID != -1){
                nameMessage(users[clientID].IP.c_str(), users[clientID].port, name.c_str(), users[i].fd);
            }
        }
        users[clientID].name = name;
    }
}