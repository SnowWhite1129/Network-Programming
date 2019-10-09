#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

#include <string>
#include <vector>

using namespace std;

int takeInput(string &str);
void execArgs(vector <string> &parsed);
void execArgsPiped(vector<vector <string> > &parsed, int pipe_count);
int parsePipe(string &str, vector <string> &strpiped);
void parseSpace(string& str, vector <string> &parsed);
int processString(string &str, vector <vector <string> > &parsed);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
