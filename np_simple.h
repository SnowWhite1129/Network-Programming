#ifndef NETWORK_PROGRAMMING_NP_SIMPLE_H
#define NETWORK_PROGRAMMING_NP_SIMPLE_H

#include <vector>
#include <string>
#include "npshell.h"

using namespace std;

int takeInput();
void printenv(const string &name);
bool Init();
void argsFree(char **args);
void execArgs(vector <string> &parsed, Symbol symbol);
void execArgsPiped(vector <string> &parsed, Symbol symbol);
void chat(const struct sockaddr_in &client);

#endif //NETWORK_PROGRAMMING_NP_SIMPLE_H