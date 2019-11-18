//
// Created by Taka on 2019/11/18.
//

#ifndef NETWORK_PROGRAMMING_MULTIMESSAGE_H
#define NETWORK_PROGRAMMING_MULTIMESSAGE_H

#include <string>
#include <vector>
#include "user.h"
#include "sharememory.h"

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
void yell(int clientID, ShareMemory *shm, const char message[]);
void tell(int sender, int receiver, const char message[], ShareMemory *shm);
void whoMessage(int clientID, const User users[]);
void who(int clientID, const User users[]);
void name(int clientID, const string &name, ShareMemory *shm);

#endif //NETWORK_PROGRAMMING_MULTIMESSAGE_H
