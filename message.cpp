#include "message.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
void welcomeMessage(int fd){
    char buffer[1025];
    sprintf(buffer, "***************************************\n"
            "** Welcome to the information server **\n"
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