#ifndef NETWORK_PROGRAMMING_MESSAGE_H
#define NETWORK_PROGRAMMING_MESSAGE_H

#endif //NETWORK_PROGRAMMING_MESSAGE_H

#include <string>
using namespace std;
void welcomeMessage();
void loginMessage(const char IP[], int port);
void logoutMessage(string name);
void yellMessage(const char name[], const char message[]);
void toldMessage(const char name[], const char message[]);
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID);
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID);
void nouserMessage(int ID);
void nomessageMessage(int senderID, int receiverID);
void occuipiedMessage(int senderID, int receiverID);