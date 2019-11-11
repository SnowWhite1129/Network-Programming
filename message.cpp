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
//    cout << "***************************************\n"
//            "** Welcome to the information server **\n"
//            "***************************************" << endl;
}
void loginMessage(const char IP[], int port, int fd){
    char buffer[1025];
    sprintf(buffer, "*** User '(no name)' entered from %s:%d. ***\n", IP, port);
    write(fd, buffer, strlen(buffer));
    //cout << "*** User '(no name)' entered from " << IP << ":" << port << ". ***" << endl;
}
void logoutMessage(const char name[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** User '%s' left. ***\n", name);
    write(fd, buffer, strlen(buffer));
    //cout << "*** User '" << name <<"' left. ***" << endl;
}
void yellMessage(const char name[], const char message[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** %s yelled ***: %s\n", name, message);
    write(fd, buffer, strlen(buffer));
    //cout << "*** " << name << " yelled ***: " << message << endl;
}
void toldMessage(const char name[], const char message[], int fd){
    char buffer[1025];
    sprintf(buffer, "*** %s told you ***: %s\n", name, message);
    write(fd, buffer, strlen(buffer));
    //cout << "*** " << name << " told you ***: " << message << endl;
}
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID, int fd){
    char buffer[1025] = {0};
    sprintf(buffer, "*** %s(#%d) just piped '%s' to %s (#%d) ***\n",
            sendername, senderID+1, message, receivername, receiverID+1);
    write(fd, buffer, strlen(buffer));
    //cout << "*** " << sendername << " " << "(#" << senderID <<  ") just piped '" << message <<" ' to " << receivername <<" (#" << receiverID << ") ***" << endl;
}
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID, int fd){
    char buffer[1025] = {0};
    sprintf(buffer, "*** %s(#%d) just received from %s (#%d) by '%s' ***\n", receivername, receiverID+1, sendername, senderID+1, message);
    write(fd, buffer, strlen(buffer));
    //cout << "*** " << receivername << " " << "(#" << receiverID <<  ") just received from '" << receivername <<" (#" << receiverID << ") " << "by '" << message << "' ***" << endl;
}
void nouserMessage(int ID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: user #%d does not exist yet. ***\n", ID);
    write(fd, buffer, strlen(buffer));
    //cout << "*** Error: user #" << ID << " does not exist yet. ***";
}
void nomessageMessage(int senderID, int receiverID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: the pipe #%d->#%d does not exist yet. ***\n", senderID+1, receiverID+1);
    write(fd, buffer, strlen(buffer));
    //cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " does not exist yet. ***" << endl;
}
void occuipiedMessage(int senderID, int receiverID, int fd){
    char buffer[1025];
    sprintf(buffer, "*** Error: the pipe #%d->#%d already exists. ***\n", senderID+1, receiverID+1);
    write(fd, buffer, strlen(buffer));
    //cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " already exists. ***" << endl;
}