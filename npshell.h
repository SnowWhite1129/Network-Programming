#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>

using namespace std;
enum Symbol {piped, numberpiped, numberexplamation, redirectout, normal};
void childHandler(int signo);
void printenv(const string &name);
void argsFree(char **args);
#endif //NETWORK_PROGRAMMING_NPSHELL_H
