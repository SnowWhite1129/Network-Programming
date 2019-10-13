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
    command(int n1, int fd1, int errfd1):n(n1), fd(fd1), errfd(errfd1){}
    ~command();
};

int takeInput();
void execArgs(vector <string> &parsed);
void execArgsPiped(vector <string> parsed, Symbol symbol);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
