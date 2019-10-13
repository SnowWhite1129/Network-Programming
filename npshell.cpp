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
        if (it->n ==0){
            vector <command>::iterator tmp;
            tmp = it;
            cmd.erase(tmp);
        }
    }
}

vector <command> check(){
    vector <command> tmp;
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd.at(i).n == 1)
            tmp.push_back(cmd.at(i));
    }
    return tmp;
}

int takeInput(){
    string line;
    int num = 0;
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
                args.push_back(str);
            } else{
                symbol = piped;
            }
        } else if (str[0] == '!'){
            symbol = numberexplamation;
            args.push_back(str);
        } else if (str[0] == '>'){
            symbol = redirectout;
            continue;
        } else {
            args.push_back(str);
            if (symbol != redirectout)
                continue;
        }
        execArgsPiped(args, symbol);
        args.clear();
        ++num;
    }

    if (num == 0){
        execArgs(args);
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
    int status;
    char *args[MAXLIST];
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
    }else if(parsed.at(0) == "flag"){
        cout << "taka{how_can_you_know_my_flaggggggggg?}" << endl;
    }

    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        cout << "Failed forking child" << endl ;
        return;
    } else if (pid == 0) {
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
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> parsed, Symbol symbol)
{
    int fd[2], status;
    pid_t pid;
    char *args[MAXLIST];

    if (pipe(fd) < 0) {
        cout << "Pipe could not be initialized" << endl;
        return;
    }

    pid = fork();
    if (pid < 0) {
        cout << "Could not fork" << endl;
        return;
    }

    if (pid==0){
        vector <command> tmp = check();
        if (!tmp.empty()){ //Looking for some command output for this command input
            for (int i = 0; i < tmp.size(); ++i) {
                dup2(tmp.at(i).fd, STDIN_FILENO);
                if (tmp.at(i).errfd != -1)
                    dup2(tmp.at(i).errfd, STDERR_FILENO);
            }
        }
        if (symbol == numberpiped || symbol == numberexplamation) {
            dup2(fd[WRITE_END], STDOUT_FILENO);
            if (symbol == numberexplamation){
                int errfd[2];
                if (pipe(errfd) < 0) {
                    cout << "Pipe could not be initialized" << endl;
                    return;
                }
                dup2(errfd[WRITE_END], STDERR_FILENO);
            }
        }
        if (symbol == redirectout){
            int out = open(parsed.at(parsed.size()-1).c_str(), O_RDWR|O_CREAT);
            if (out == -1){
                cout << "File open error." << endl;
                return;
            }
        }

        close(fd[READ_END]);
        close(fd[WRITE_END]);

        for(int i=0; i<parsed.size();i++){
            //cout << j;
            args[i] = strdup(parsed.at(i).c_str());
            //cout << args[j] << endl;
        }

        //cout << parsed.at(i).size() << endl;
        //cout << "HI " << parsed.at(i).at(0) << endl;
        //args[parsed.at(i).size()] = NULL;

        if (execvp(parsed.at(0).c_str(), args) < 0) {
            cout << "Could not execute [" << parsed.at(0) << "]." << endl;
            argsFree(args);
        }
        exit(0);
    }

    waitpid(pid, &status, 0);
    argsFree(args);
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