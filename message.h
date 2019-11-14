#ifndef NETWORK_PROGRAMMING_MESSAGE_H
#define NETWORK_PROGRAMMING_MESSAGE_H

#include <string>
#include <vector>
#include "user.h"
#include "sharememory.h"
using namespace std;
void welcomeMessage(int fd);
void loginMessage(const char IP[], int port, int fd);
void logoutMessage(const char name[], int fd);
void yellMessage(const char name[], const char message[], int fd);
void toldMessage(const char name[], const char message[], int fd);
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID, int fd);
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID, int fd);
void nouserMessage(int ID, int fd);
void nomessageMessage(int senderID, int receiverID, int fd);
void occuipiedMessage(int senderID, int receiverID, int fd);
void nameMessage(const char IP[], int port, const char name[], int fd);
void duplicatNameMessage(const char name[], int fd);
void whoMessage(int clientID, const User users[], int fd);
void userMessage(const User &user, int fd);
void login(int newclient, const User users[]);
void logout(int newclient, const User users[]);
void recieve(int receiverID, int senderID, const string &message, const User users[]);
void send(int senderID, int receiverID, const string &message, const User users[]);
void yell(int clientID, const string &message, const User users[]);
void tell(int sender, int receiver, const string &message, const User users[]);
void who(int clientID, const User users[]);
void name(int clientID, const string &name, User users[]);


void welcomeMessage();
void loginMessage(const char IP[], int port, ShareMemory *shm, int senderID, int clientID);
void logoutMessage(const char name[], ShareMemory *shm, int senderID, int clientID);
void yellMessage(const char name[], const char message[], ShareMemory *shm, int senderID, int clientID);
void toldMessage(const char name[], const char message[], ShareMemory *shm, int senderID, int clientID);
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID, ShareMemory *shm);
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID, ShareMemory *shm);
void nouserMessage(int ID);
void nomessageMessage(int senderID, int receiverID);
void occuipiedMessage(int senderID, int receiverID);
void userMessage(const User &user);
void duplicatNameMessage(const string& name);
void nameMessage(int clientID, const char IP[], int port, const char name[], ShareMemory *shm);
void login(int newclient, ShareMemory *shm);
void logout(int newclient, ShareMemory *shm);
void recieve(int receiverID, int senderID, const string &message, ShareMemory *shm);
void send(int senderID, int receiverID, const string &message, ShareMemory *shm);
void yellmulti(int clientID, ShareMemory *shm, const char message[]);
void tellmulti(int sender, int receiver, const char message[], ShareMemory *shm);
void whoMessagemulti(int clientID, const User users[]);
void whomulti(int clientID, const User users[]);
void name(int clientID, const string &name, ShareMemory *shm);


#endif //NETWORK_PROGRAMMING_MESSAGE_H

