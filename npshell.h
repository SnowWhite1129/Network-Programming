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
    Symbol symbol;
    void Init(int n, int fd, int errfd, Symbol symbol);
};

void command::Init(int n, int fd, int errfd, Symbol symbol) {
    n = n;
    fd = fd;
    errfd = errfd;
    symbol = symbol;
}

int takeInput();
void execArgs(vector <string> &parsed);
void execArgsPiped(vector <string> parsed, Symbol symbol);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
