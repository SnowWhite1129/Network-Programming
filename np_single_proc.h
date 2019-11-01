#ifndef NETWORK_PROGRAMMING_NP_SINGLE_PROC_H
#define NETWORK_PROGRAMMING_NP_SINGLE_PROC_H

#endif //NETWORK_PROGRAMMING_NP_SINGLE_PROC_H
#include <vector>
#include <string>
#include "npshell.h"
#include "user.h"

using namespace std;

int takeInput();
void printenv(const string &name);
bool Init(User users[]);
void argsFree(char **args);
void execArgs(vector <string> &parsed, Symbol symbol);
void execArgsPiped(vector <string> &parsed, Symbol symbol);
void chat(const struct sockaddr_in &client);