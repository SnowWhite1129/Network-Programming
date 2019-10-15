#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>
#include <vector>
using namespace std;

enum Symbol {piped, numberpiped, numberexplamation, redirectout, normal};

struct command{
    int n;
    int fd[2];
    void Init(int n1, int fd1[2]);
};
void command::Init(int n1, int fd1[2]) {
    n = n1;
    for (int i = 0; i < 2; ++i) {
        fd[i] = fd1[i];
    }
}
int takeInput();
void execArgs(vector <string> &parsed, Symbol symbol);
void execArgsPiped(vector <string> parsed, Symbol symbol);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
