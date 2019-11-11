#include "nppipe.h"

bool checkPipeExist(int sender, int clientID, Pipe pipe_table[][max_clients]){
    for (int i = 0; i < max_clients; ++i) {
        if (i != sender ){
            if (pipe_table[i][clientID].readfd != -1 || pipe_table[i][clientID].writefd != -1){
                return true;
            }
        }
    }
    return false;
}
bool checkPipeStatus(int sender, int clientID, Pipe pipe_table[][max_clients]){
    return pipe_table[sender][clientID].readfd == -1 || pipe_table[sender][clientID].writefd == -1;
}