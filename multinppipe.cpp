//
// Created by seed on 19年11月18日.
//
#include "multinppipe.h"

bool checkPipeStatusMulti(int sender, int clientID, const bool pipe_status[][max_clients]){
    return pipe_status[sender][clientID];
}
