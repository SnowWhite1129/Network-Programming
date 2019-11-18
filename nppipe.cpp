#include "nppipe.h"
#include <unistd.h>

bool checkPipeExist(int sender, int clientID, Pipe pipe_table[][max_clients]){
    return pipe_table[sender][clientID].readfd != -1 || pipe_table[sender][clientID].writefd != -1;
}
bool checkPipeStatus(int sender, int clientID, Pipe pipe_table[][max_clients]){
    return pipe_table[sender][clientID].readfd == -1 || pipe_table[sender][clientID].writefd == -1;
}
bool checkPipeStatusMulti(int sender, int clientID, bool pipe_status[][max_clients]){
    return pipe_status[sender][clientID];
}
void closePipe(int clientID, Pipe pipe_table[][max_clients]){
    for (int i = 0; i < max_clients; ++i) {
        if (pipe_table[i][clientID].readfd != -1 && pipe_table[i][clientID].writefd != -1){
            close(pipe_table[i][clientID].readfd);
            close(pipe_table[i][clientID].writefd);
            pipe_table[i][clientID].readfd = -1;
            pipe_table[i][clientID].writefd = -1;
        }
    }
}