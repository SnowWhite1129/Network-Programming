#ifndef NETWORK_PROGRAMMING_MESSAGE_H
#define NETWORK_PROGRAMMING_MESSAGE_H

#include <string>
#include <vector>
#include "user.h"

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
void yell(int clientID, const vector<string> &parsed, const User users[]);
void tell(int sender, int receiver, const vector<string> &parsed, const User users[]);
void who(int clientID, const User users[]);
void name(int clientID, const string &name, User users[]);



#endif //NETWORK_PROGRAMMING_MESSAGE_H

