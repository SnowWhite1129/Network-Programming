#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>
#include <vector>
using namespace std;

enum Symbol {piped, numberpiped, numberexplamation, redirectout, normal};

struct command{
    int fd[2];
    command &operator = (const command &tmp);
    void Init(const int fd1[2]);
    void Clean();
};
struct User{
    string name, IP;
    int ID, port;
    static int n;
    void Init(string IP1, int ID1, int port1);
    void Delete();
};

int takeInput();
void execArgs(vector <string> &parsed, Symbol symbol);
void execArgsPiped(vector <string> &parsed, Symbol symbol);
void Pop();
int check(int n);
void printenv(const string &name);
bool Init(User users[]);
void argsFree(char **args);
void childHandler(int signo);
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

#endif //NETWORK_PROGRAMMING_NPSHELL_H
