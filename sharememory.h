#ifndef NETWORK_PROGRAMMING_SHAREMEMORY_H
#define NETWORK_PROGRAMMING_SHAREMEMORY_H

#include "multiuser.h"

struct ShareMemory{
    User users[max_clients];
    bool userstatus;
    bool pipe_status[max_clients][max_clients];
    char message[max_clients][max_clients][1025];
    int pipe_fd[max_clients][max_clients];
    int n;
};

#endif //NETWORK_PROGRAMMING_SHAREMEMORY_H
