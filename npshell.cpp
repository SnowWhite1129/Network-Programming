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

void check(vector <command> &tmp){
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd.at(i).n == 0)
            tmp.push_back(cmd.at(i));
    }
    fprintf(stderr, "tmp: %d\n", tmp.size());
}

void dupinput(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
		fprintf(stderr, "fd: %d %d\n" ,tmp.at(i).fd, STDIN_FILENO);
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
            continue;
        }
        execArgsPiped(args, symbol);
        Count();
        Pop();
        args.clear();
    }

    fprintf(stderr, "Size: %d\n", args.size());

    if (symbol == normal || symbol == redirectout){
        execArgs(args, symbol);
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
void execArgs(vector <string> &parsed, Symbol symbol){
    if(parsed.at(0)=="exit"){
        exit(0);
    }else if (parsed.at(0)== "setenv"){
        if(!setenv(parsed.at(0).c_str(), parsed.at(1).c_str(), true)){
            cout << "Set env Error" << endl;
            exit(0);
        }
        return;
    }else if(parsed.at(0) == "printenv"){
    	printenv(parsed.at(0));
	    return;
    }

    if (symbol == redirectout){
        int out = open(parsed.at(parsed.size()-1).c_str(), O_RDWR|O_CREAT);
        if (out == -1){
            cout << "File open error." << endl;
            return;
        }
    }

    // Forking a child
    pid_t pid = fork();
    vector <command> tmp;
    check(tmp);
    if (pid == -1) {
        cout << "Failed forking child" << endl ;
        return;
    } else if (pid == 0) {
	    fprintf(stderr, "ttttmp: %d\n", tmp.size());
        dupinput(tmp);
	    dupclose(tmp);
        char *args[MAXLIST];
	    for(int i=0; i<parsed.size();i++){
            args[i] = strdup(parsed.at(i).c_str());
        }
        args[parsed.size()] = NULL;
	    fprintf(stderr, "9999999999");
        if (execvp(args[0], args) < 0) {
            cout << "Unknown command: [" << args[0] << "]." << endl;
        }
	    fprintf(stderr, "555555");
        argsFree(args);
        exit(0);
    } else {
        fprintf(stderr, "123213213213\n");
	    dupclose(tmp);
        int status;
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> parsed, Symbol symbol)
{
    int fd[2], errfd[2];
    pid_t pid;

    if (pipe(fd) < 0) {
        cout << "Pipe could not be initialized" << endl;
        return;
    }
    if (symbol == piped || symbol == numberpiped || symbol == numberexplamation) {
        int n = 1, err = -1;
        if (symbol != piped)
            n = std::stoi(parsed.at(parsed.size()-1));
        //cout <<  "N: " << n << endl;
        if (symbol == numberexplamation){

            if (pipe(errfd) < 0) {
                cout << "Pipe could not be initialized" << endl;
                return;
            }
            err = errfd[READ_END];
        }
        command tmpcmd;
        tmpcmd.Init(n, fd[READ_END], err);
        cmd.push_back(tmpcmd);
    }
    fprintf(stderr, "Hey");

    pid = fork();
    if (pid < 0) {
        cout << "Could not fork" << endl;
        return;
    }
    vector <command> tmp;
    check(tmp);
    if (pid==0){
        dup2(fd[WRITE_END], STDOUT_FILENO);
        dupinput(tmp);
        dupclose(tmp);
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        if(symbol == numberexplamation){
            dup2(errfd[WRITE_END], STDERR_FILENO);
            close(errfd[READ_END]);
            close(errfd[WRITE_END]);
        }
	    
        char *args[MAXLIST];

        int length;
        if (symbol == numberpiped || symbol == numberexplamation)
            length = parsed.size()-1;
        else
            length = parsed.size();

        for(int i=0; i<length;i++){
            fprintf(stderr, "%d ", i);
            args[i] = strdup(parsed.at(i).c_str());
            fprintf(stderr, "%s " ,args[i]);
        }

        fprintf(stderr, "%d", length);
        fprintf(stderr, "HI %s\n", args[0]);
        args[length] = NULL;

        if (execvp(args[0], args) < 0) {
            cout << "Could not execute [" << args[0] << "]." << endl;
            argsFree(args);
        }
        fprintf(stderr, "HI");
        exit(0);
    } else {
        int status;
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        if(symbol == numberexplamation){
            close(errfd[READ_END]);
            close(errfd[WRITE_END]);
        }
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
