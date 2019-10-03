
#ifndef NETWORK_PROGRAMMING_NPSHELL_H
#define NETWORK_PROGRAMMING_NPSHELL_H

int takeInput(char* str);
void execArgs(char** parsed);
void execArgsPiped(char** parsed, char** parsedpipe);
int parsePipe(char* str, char** strpiped);
void parseSpace(char* str, char** parsed);
int processString(char* str, char** parsed, char** parsedpipe);

#endif //NETWORK_PROGRAMMING_NPSHELL_H
