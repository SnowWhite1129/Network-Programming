#include "command.h"
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
void Pop(command cmd[]){
    for (int i=0; i<MAXLIST-1;i++)
        cmd[i] = cmd[i+1];
    cmd[MAXLIST-1].Clean();
}

int check(const command cmd[], int n){
    if (cmd[n].fd[READ_END]!=-1)
        return cmd[n].fd[READ_END];
    else
        return -1;
}