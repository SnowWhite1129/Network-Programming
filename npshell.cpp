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
    for (it = cmd.begin(); it!=cmd.end();) {
        if (it->n == -1){
            it = cmd.erase(it);
        } else {
	    it++;
	} 
    }
}

void check(vector <command> &tmp){
    for (int i = 0; i < cmd.size(); ++i) {
        if (cmd.at(i).n == 0)
            tmp.push_back(cmd.at(i));
    }
    //fprintf(stderr, "tmp: %d\n", tmp.size());
}

void dupinput(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
		    //fprintf(stderr, "fd: %d %d\n" ,tmp.at(i).fd[READ_END], STDIN_FILENO);
            dup2(tmp.at(i).fd[READ_END], STDIN_FILENO);
            if (tmp.at(i).errfd[READ_END] != -1 && tmp.at(i).errfd[WRITE_END] != -1)
                dup2(tmp.at(i).errfd[READ_END], STDERR_FILENO);
        }
    }
}

void dupcloseread(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
            close(tmp.at(i).fd[READ_END]);
            if (tmp.at(i).errfd[READ_END] != -1 && tmp.at(i).errfd[WRITE_END] != -1)
                close(tmp.at(i).errfd[READ_END]);
        }
    }
}

void dupclosewrite(vector <command> &tmp){
    if (!tmp.empty()){ //Looking for some command output for this command input
        for (int i = 0; i < tmp.size(); ++i) {
            close(tmp.at(i).fd[WRITE_END]);
            if (tmp.at(i).errfd[READ_END] != -1 && tmp.at(i).errfd[WRITE_END] != -1)
                close(tmp.at(i).errfd[READ_END]);
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
            if (symbol != redirectout)
                symbol = normal;
            continue;
        }
        execArgsPiped(args, symbol);
        Count();
        Pop();
        args.clear();
    }

    //fprintf(stderr, "Size: %d\n", args.size());

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
	    //fprintf(stderr, "ttttmp: %d\n", tmp.size());
        dupclosewrite(tmp);
	    dupinput(tmp);
	    dupcloseread(tmp);
        char *args[MAXLIST];
	    for(int i=0; i<parsed.size();i++){
            args[i] = strdup(parsed.at(i).c_str());
        }
        args[parsed.size()] = NULL;
	    //fprintf(stderr, "9999999999");
        if (execvp(args[0], args) < 0) {
            cout << "Unknown command: [" << args[0] << "]." << endl;
        }
	    fprintf(stderr, "555555");
        argsFree(args);
        exit(0);
    } else {
        //fprintf(stderr, "123213213213\n");
        dupclosewrite(tmp);
        dupcloseread(tmp);
        int status;
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> parsed, Symbol symbol)
{
    int fd[2] , errfd[2];
    for (int i = 0; i < 2; ++i) {
        errfd[i] = -1;
    }
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
        }
        command tmpcmd;
        tmpcmd.Init(n, fd, errfd);
        cmd.push_back(tmpcmd);
    }
    //fprintf(stderr, "Hey");

    pid = fork();
    if (pid < 0) {
        cout << "Could not fork" << endl;
        return;
    }
    vector <command> tmp;
    check(tmp);
    if (pid==0){
        close(fd[READ_END]);
        dup2(fd[WRITE_END], STDOUT_FILENO);
        close(fd[WRITE_END]);
        dupclosewrite(tmp);
        dupinput(tmp);
        dupcloseread(tmp);
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
            //fprintf(stderr, "%d ", i);
            args[i] = strdup(parsed.at(i).c_str());
            //fprintf(stderr, "%s " ,args[i]);
        }

        //fprintf(stderr, "%d", length);
        //fprintf(stderr, "HI %s\n", args[0]);
        args[length] = NULL;

        if (execvp(args[0], args) < 0) {
            cout << "Could not execute [" << args[0] << "]." << endl;
        }
        argsFree(args);
        //fprintf(stderr, "HI");
        exit(0);
    } else {
        int status;
        dupclosewrite(tmp);
        dupcloseread(tmp);
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
