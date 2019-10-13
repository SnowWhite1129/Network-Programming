#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>
#include <vector>
using namespace std;

enum Symbol {piped, numberpiped, numberexplamation, redirectout, normal};

struct command{
    int n;
    int fd;
    int errfd;
    void Init(int n, int fd, int errfd);
};

void command::Init(int n, int fd, int errfd) {
    n = n;
    fd = fd;
    errfd = errfd;
}

int takeInput();
void execArgs(vector <string> &parsed);
void execArgsPiped(vector <string> parsed, Symbol symbol);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
