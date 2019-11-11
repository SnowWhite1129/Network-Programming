#ifndef NETWORK_PROGRAMMING_NPPIPE_H
#define NETWORK_PROGRAMMING_NPPIPE_H

#include "user.h"

struct Pipe{
    int readfd, writefd;
};
bool checkPipeExist(int sender, int clientID, Pipe pipe_table[][max_clients]);
bool checkPipeStatus(int sender, int clientID, Pipe pipe_table[][max_clients]);

#endif //NETWORK_PROGRAMMING_NPPIPE_H
