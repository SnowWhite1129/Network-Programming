
#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>
#include <vector>

#define MAXLIST 256

using namespace std;

int takeInput(string &str);
void execArgs(vector <string> parsed);
void execArgsPiped(vector <string> parsed[MAXLIST], int pipe_count);
int parsePipe(string &str, vector <string> strpiped);
void parseSpace(string& str, vector <string> parsed);
int processString(string &str, vector <string> parsed[MAXLIST]);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
