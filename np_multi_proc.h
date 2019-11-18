#ifndef NETWORK_PROGRAMMING_NP_MULTI_PROC_H
#define NETWORK_PROGRAMMING_NP_MULTI_PROC_H

#endif //NETWORK_PROGRAMMING_NP_MULTI_PROC_H
#include <vector>
#include <string>
#include "npshell.h"
#include "user.h"

using namespace std;

void messageHandler(int signo);
void fifoHandler(int signo);
int takeInput(int clientID);
void execArgs(vector <string> &parsed, Symbol symbol, int clientID, int sender, const string &line);
void execArgsPiped(vector <string> &parsed, Symbol symbol, int clientID, int sender, int receiver, const string &line);
void chat(int clientID);