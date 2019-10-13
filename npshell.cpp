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

vector<command> cmd;

void Count(){
    for (int i = 0; i < cmd.size(); ++i) {
        --cmd.at(i).n;
    }
}
void Pop(){
    //TODO
    //Need to test
    vector <command>::iterator it;
    for (it = cmd.begin(); it!=cmd.end() ; ++it) {
        if (it->n == -1){
            vector <command>::iterator tmp;
            tmp = it;
            cmd.erase(tmp);
        }
    }
}

vector <command> check(){
    vector <command> tmp;
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd.at(i).n == 0)
            tmp.push_back(cmd.at(i));
    }
    return tmp;
}

void dupinput(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
            dup2(tmp.at(i).fd, STDIN_FILENO);
            if (tmp.at(i).errfd != -1)
                dup2(tmp.at(i).errfd, STDERR_FILENO);
        }
    }
}

void dupclose(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
            close(tmp.at(i).fd);
            if (tmp.at(i).errfd != -1)
                close(tmp.at(i).errfd);
        }
    }
}

int takeInput(){
    string line;
    Symbol symbol = normal;
    getline(cin, line);

    if (line.length()==0){
        return false;
    }

    istringstream iss(line);
    string str;

    vector <string> args;

    while (iss >> str){
        if (str[0] == '|'){
            if (str.length()>1){
                symbol = numberpiped;
                str = str.substr(1);
                args.push_back(str);
            } else{
                symbol = piped;
            }
        } else if (str[0] == '!'){
            symbol = numberexplamation;
            str = str.substr(1);
            args.push_back(str);
        } else if (str[0] == '>'){
            symbol = redirectout;
            continue;
        } else {
            args.push_back(str);
            if (symbol != redirectout) {
                symbol = normal;
                continue;
            }
        }
        execArgsPiped(args, symbol);
        Count();
        Pop();
        args.clear();
    }

    if (symbol == normal){
        execArgs(args);
        Count();
        Pop();
    }
    return true;
}

void printenv(string &name){
	cout << getenv(name.c_str()) << endl;
}
bool Init(){
	return setenv("PATH", "bin:.", true);
}

void argsFree(char **args){
    for(int i=0;args[i]!=NULL;++i){
        free(args[i]);
    }
}
// Function where the system command is executed
void execArgs(vector <string> &parsed)
{
    if(parsed.at(0)=="exit"){
        exit(0);
    }else if (parsed.at(0)== "setenv"){
        if(!setenv(parsed.at(1).c_str(), parsed.at(2).c_str(), true)){
            cout << "Set env Error" << endl;
            exit(0);
        }
        return;
    }else if(parsed.at(0) == "printenv"){
    	printenv(parsed.at(0));
	    return;
    }

    // Forking a child
    pid_t pid = fork();
    vector <command> tmp;
    if (pid == -1) {
        cout << "Failed forking child" << endl ;
        return;
    } else if (pid == 0) {
        tmp = check();
        dupinput(tmp);
        char *args[MAXLIST];
	    for(int i=0; i<parsed.size();i++){
            args[i] = strdup(parsed.at(i).c_str());
        }
        args[parsed.size()] = NULL;
        if (execvp(parsed.at(0).c_str(), args) < 0) {
            cout << "Unknown command: [" << parsed.at(0) << "]." << endl;
        }
        argsFree(args);
        exit(0);
    } else {
        int status;
        dupclose(tmp);
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> parsed, Symbol symbol)
{
    int fd[2], status;
    pid_t pid;

    if (pipe(fd) < 0) {
        cout << "Pipe could not be initialized" << endl;
        return;
    }

    pid = fork();
    if (pid < 0) {
        cout << "Could not fork" << endl;
        return;
    }

    vector <command> tmp = check();
    dupinput(tmp);
    if (symbol == piped || symbol == numberpiped || symbol == numberexplamation) {
        dup2(fd[WRITE_END], STDOUT_FILENO);
        command tmp;
        int n = 1, err = -1;
        if (symbol != piped)
            n = stoi(parsed.at(parsed.size()-1).c_str());
        cout <<  "N: " << n << endl;
        if (symbol == numberexplamation){
            int errfd[2];
            if (pipe(errfd) < 0) {
                cout << "Pipe could not be initialized" << endl;
                return;
            }
            dup2(errfd[WRITE_END], STDERR_FILENO);
            err = errfd[READ_END];
        }
        tmp.Init(n, fd[READ_END], err);
        cmd.push_back(tmp);
    }
    if (symbol == redirectout){
        int out = open(parsed.at(parsed.size()-1).c_str(), O_RDWR|O_CREAT);
        if (out == -1){
            cout << "File open error." << endl;
            return;
        }
    }

    if (pid==0){
        char *args[MAXLIST];

        int length;
        if (symbol == numberpiped || symbol == numberexplamation)
            length = parsed.size()-1;
        else
            length = parsed.size();

        for(int i=0; i<length;i++){
            cout << i;
            args[i] = strdup(parsed.at(i).c_str());
            cout << args[i] << endl;
        }

        cout << length << endl;
        cout << "HI " << parsed.at(0) << endl;
        args[length] = NULL;

        if (execvp(parsed.at(0).c_str(), args) < 0) {
            cout << "Could not execute [" << parsed.at(0) << "]." << endl;
            argsFree(args);
        }
        exit(0);
    } else {
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        dupclose(tmp);
        waitpid(pid, &status, 0);
    }
}

int main(){
    if(Init()){
	    printf("Init error\n");
	    exit(0);
    }

    while (true) {
        printf("%% ");
        if (!takeInput())
            continue;
    }
}