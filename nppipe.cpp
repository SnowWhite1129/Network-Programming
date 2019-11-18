#include "nppipe.h"
#include <unistd.h>

bool checkPipeStatus(int sender, int clientID, const Pipe pipe_table[][max_clients]){
    return pipe_table[sender][clientID].readfd == -1 || pipe_table[sender][clientID].writefd == -1;
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