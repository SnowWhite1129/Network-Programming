#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>

using namespace std;
enum Symbol {piped, numberpiped, numberexplamation, redirectout, userpipe, normal};
void childHandler(int signo);
void printenv(const string &name);
void printenv(const string &name, int fd);
void argsFree(char **args);
void exitHandler(int signo);
#endif //NETWORK_PROGRAMMING_NPSHELL_H
