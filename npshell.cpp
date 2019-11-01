#include <wait.h>
#include <iostream>
#include "npshell.h"

void childHandler(int signo){
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
    // NON-BLOCKING WAIT
    // Return immediately if no child has exited.
}
void printenv(const string &name){
    cout << getenv(name.c_str()) << endl;
}
void argsFree(char **args){
    for(int i=0;args[i]!= nullptr;++i)
        free(args[i]);
}
