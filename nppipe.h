#ifndef NETWORK_PROGRAMMING_NPPIPE_H
#define NETWORK_PROGRAMMING_NPPIPE_H

#include "user.h"

struct Pipe{
    int readfd, writefd;
};
bool checkPipeStatus(int sender, int clientID, const Pipe pipe_table[][max_clients]);
void closePipe(int clientID, const Pipe pipe_table[][max_clients]);
bool checkPipeStatusMulti(int sender, int clientID, const bool pipe_status[][max_clients]);
#endif //NETWORK_PROGRAMMING_NPPIPE_H
