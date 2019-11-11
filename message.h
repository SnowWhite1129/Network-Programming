#ifndef NETWORK_PROGRAMMING_MESSAGE_H
#define NETWORK_PROGRAMMING_MESSAGE_H

#endif //NETWORK_PROGRAMMING_MESSAGE_H

#include <string>
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