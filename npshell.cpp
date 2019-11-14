#include <wait.h>
#include <iostream>
#include "npshell.h"
#include <unistd.h>

void childHandler(int signo){
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
    // NON-BLOCKING WAIT
    // Return immediately if no child has exited.
}
void printenv(const string &name){
    cout << getenv(name.c_str()) << endl;
}
void printenv(const string &name, int fd){
    string message = getenv(name.c_str());
    message += "\n";
    write(fd, message.c_str(), message.size());
}
void argsFree(char **args){
    for(int i=0;args[i]!= nullptr;++i)
        free(args[i]);
}
void exitHandler(int signo){
    exit(0);
}
