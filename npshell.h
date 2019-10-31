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
void command::Init(const int fd1[2]) {
    for (int i = 0; i < 2; ++i) {
        fd[i] = fd1[i];
    }
}
command& command::operator=(const command &tmp) {
    for (int i = 0; i < 2; ++i) {
        fd[i] = tmp.fd[i];
    }
}
void command::Clean() {
    for (int & i : fd) {
        i = -1;
    }
}

int takeInput();
void execArgs(vector <string> &parsed, Symbol symbol);
void execArgsPiped(vector <string> &parsed, Symbol symbol);
void Pop();
int check(int n);
void printenv(const string &name);
bool Init();
void argsFree(char **args);
void func(int sockfd);
void childHandler(int signo);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
