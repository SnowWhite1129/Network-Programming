//
// Created by seed on 19年11月18日.
//

#ifndef NETWORK_PROGRAMMING_MULTINPPIPE_H
#define NETWORK_PROGRAMMING_MULTINPPIPE_H

#include "multiuser.h"

struct Pipe{
    int readfd, writefd;
};

bool checkPipeStatusMulti(int sender, int clientID, const bool pipe_status[][max_clients]);

#endif //NETWORK_PROGRAMMING_MULTINPPIPE_H
