#ifndef NETWORK_PROGRAMMING_NPPIPE_H
#define NETWORK_PROGRAMMING_NPPIPE_H

#include "user.h"

struct Pipe{
    int readfd, writefd;
};
bool checkPipeExist(int sender, int clientID, Pipe pipe_table[][max_clients]);
bool checkPipeStatus(int sender, int clientID, Pipe pipe_table[][max_clients]);
void closePipe(int clientID, Pipe pipe_table[][max_clients]);
bool checkPipeExistMulti(int sender, int clientID, int pipe_fd[][max_clients]);
bool checkPipeStatusMulti(int sender, int clientID, bool pipe_status[][max_clients]);
#endif //NETWORK_PROGRAMMING_NPPIPE_H
