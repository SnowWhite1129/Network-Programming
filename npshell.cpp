#include<string>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <sstream>
#include<sys/types.h>
#include<sys/wait.h>
#include<iostream>
#include <fcntl.h>
#include "npshell.h"

#define MAXLIST 1000

#define READ_END 0
#define WRITE_END 1

using namespace std;

command cmd[MAXLIST];

void Pop(){
    for (int i=0; i<MAXLIST-1;i++)
        cmd[i] = cmd[i+1];
    cmd[MAXLIST-1].Clean();
}

int check(int n){
    if (cmd[n].fd[READ_END]!=-1)
        return cmd[n].fd[READ_END];
    else
        return -1;
}

int takeInput(){
    string line;
    Symbol symbol = normal;
    if (!getline(cin, line))
        exit(0);

    if (line.length()==0)
        return false;

    istringstream iss(line);
    string str;

    vector <string> args;

    while (iss >> str){
        if (str[0] == '|'){
            if (str.length()>1){
                symbol = numberpiped;
                str = str.substr(1);
                args.push_back(str);
            } else
                symbol = piped;
        } else if (str[0] == '!'){
            symbol = numberexplamation;
            str = str.substr(1);
            args.push_back(str);
        } else if (str[0] == '>'){
            symbol = redirectout;
            continue;
        } else {
            args.push_back(str);
            if (symbol != redirectout)
                symbol = normal;
            continue;
        }
        execArgsPiped(args, symbol);
        Pop();
        args.clear();
    }

    if (symbol == normal || symbol == redirectout){
        execArgs(args, symbol);
        Pop();
    }
    return true;
}

void printenv(const string &name){
	cout << getenv(name.c_str()) << endl;
}

bool Init(){
    for (auto & i : cmd)
        i.Clean();
	return setenv("PATH", "bin:.", true)!=-1;
}

void argsFree(char **args){
    for(int i=0;args[i]!= nullptr;++i)
        free(args[i]);
}
// Function where the system command is executed
void execArgs(vector <string> &parsed, Symbol symbol){
    if (parsed.at(0)=="exit"){
        exit(0);
    } else if (parsed.at(0)== "setenv"){
        if(setenv(parsed.at(1).c_str(), parsed.at(2).c_str(), true)==-1){
            cout << "Set env Error" << endl;
            exit(0);
        }
    } else if(parsed.at(0) == "printenv"){
    	printenv(parsed.at(1));
	    return;
    }

    // Forking a child
    pid_t pid = fork();
    if (pid == -1) {
        cout << "Failed forking child" << endl ;
        return;
    } else if (pid == 0) {
	    if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            dup2(cmd[0].fd[READ_END], STDIN_FILENO);
            close(cmd[0].fd[READ_END]);
	    }

        if (symbol == redirectout){
            int out = open(parsed.at(parsed.size()-1).c_str(), O_RDWR|O_CREAT);
            if (out == -1){
                cout << "File open error." << endl;
                return;
            }
            close(STDOUT_FILENO);
            dup2(out, STDOUT_FILENO);
            close(out);
        }

        for (int i = 3; i < 1024; ++i)
            close(i);

        char *args[MAXLIST];
        int length;
        if (symbol == redirectout)
            length = parsed.size()-1;
        else
            length = parsed.size();
	    for(int i=0; i<length;i++)
            args[i] = strdup(parsed.at(i).c_str());

        args[length] = nullptr;
        if (execvp(args[0], args) < 0)
            cout << "Unknown command: [" << args[0] << "]." << endl;
        argsFree(args);
        exit(0);
    } else {
        if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            close(cmd[0].fd[READ_END]);
        }
        int status;
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> &parsed, Symbol symbol)
{
    int fd[2], n=1;
    pid_t pid;

    if (symbol != piped)
        n = stoi(parsed.at(parsed.size()-1));

    int prevfd = check(n);

    if (prevfd == -1) {
        if (pipe(fd) < 0) {
            cout << "Pipe could not be initialized" << endl;
            return;
        }
        cmd[n].Init(fd);
    } else {
        for (int i = 0; i < 2; ++i)
            fd[i] = cmd[n].fd[i];
    }

    pid = fork();
    if (pid < 0) {
        cout << "Could not fork" << endl;
        return;
    }

    if (pid==0){
        close(fd[READ_END]);
        dup2(fd[WRITE_END], STDOUT_FILENO);
        if(symbol == numberexplamation)
            dup2(fd[WRITE_END], STDERR_FILENO);
        close(fd[WRITE_END]);
        if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            dup2(cmd[0].fd[READ_END], STDIN_FILENO);
            close(cmd[0].fd[READ_END]);
        }

        for (int i = 3; i < 1024; ++i)
            close(i);
	    
        char *args[MAXLIST];

        int length;
        if (symbol == numberpiped || symbol == numberexplamation)
            length = parsed.size()-1;
        else
            length = parsed.size();

        for(int i=0; i<length;i++)
            args[i] = strdup(parsed.at(i).c_str());

        args[length] = NULL;

        if (execvp(args[0], args) < 0)
            cout << "Could not execute [" << args[0] << "]." << endl;
        argsFree(args);
        exit(0);
    } else {
        if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            close(cmd[0].fd[READ_END]);
        }
        if (symbol != piped){
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

int main(){
    if(!Init()){
	    printf("Init error\n");
	    exit(0);
    }

    while (true) {
        printf("%% ");
        if (!takeInput())
            continue;
    }
}
