#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<iostream>
#include "npshell.h"

#define READ_END 0
#define WRITE_END 1

struct command{
	int n;
	int fd;
};

int takeInput(string &str){

    getline(cin, str);
	
    if (str.length() > 0) {
        return 0;
    } else {
        return 1;
    }
}

void printenv(string &name){
	cout << getenv(name.c_str()) << endl;
}
bool Init(){
	return setenv("PATH", "bin:.", true);
}

// Function where the system command is executed
void execArgs(vector <string> &parsed)
{
    char *args[MAXLIST];
    if(parsed.at(0)=="exit"){
        exit(0);
    }else if (parsed.at(0)== "setenv"){
	if(!setenv(parsed.at(1).c_str(), parsed.at(2).c_str(), true)){
		cout << "Set env Error" << endl ;
		exit(0);
	}
	return;

    }else if(parsed.at(0) == "printenv"){
    	printenv(parsed.at(0));
	return;
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
        exit(0);
    } else {
        //TODO
        wait(0);
        return;
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> parsed[MAXLIST], int pipe_count)
{
    // 0 is read end, 1 is write end
    int fd[pipe_count][2], status;
    pid_t p1, p2;
    char *args[MAXLIST];

    for(int i = 0;i < pipe_count;++i){
        if (pipe(fd[i]) < 0) {
            cout << "Pipe could not be initialized" << endl;
            return;
        }
    }

    for(int i = 0;i <= pipe_count;++i){
        p1 = fork();
        if (p1 < 0) {
            cout << "Could not fork" << endl;
            return;
        }

        if(i != 0)
            dup2(fd[i-1][0], 0);

        if(i != pipe_count)
            dup2(fd[i][1], 1);


        for(int j = 0;j < pipe_count;j++){
            close(fd[j][0]);
            close(fd[j][1]);
        }
	for(int j=0; j<parsed[i].size();j++){
	    args[j] = strdup(parsed[i].at(j).c_str());
	}
	args[parsed[i].size()] = NULL;
        if (execvp(parsed[i].at(0).c_str(), args) < 0) {
            cout << "Could not execute [" << parsed[i].at(0) << "]." << endl;
            exit(0);
        }
    }
    for(int i = 0;i < pipe_count;i++){
        close(fd[i][0]);
        close(fd[i][1]);
    }
    waitpid(p1, &status, 0);
}

// It's assumption is only contain one pipe
// function for finding pipe
int parsePipe(string &str, vector <string> &strpiped)
{
    string tmp = str;
    size_t pos;
    int n=0;
    for(int i=0; i < tmp.length(); ++i){	
        switch(tmp[i]){
        case '|':
            //if(isdigit(tmp[i+1]))
               // cout << "number piped" << endl;//numberpipe
            pos = tmp.find("|");
	    strpiped.push_back(tmp.substr(0, pos));
	    tmp = tmp.substr(pos+1);
	    cout << tmp << endl;
	    i=-1;
            break;
        case '!':
            pos = tmp.find("!");
	    strpiped.push_back(tmp.substr(0, pos));
	    tmp = tmp.substr(pos+1);
	    i=-1;
            break;
        case '>':
            pos = tmp.find(">");
	    strpiped.push_back(tmp.substr(0, pos));
	    tmp = tmp.substr(pos+1);
	    i=-1;
            break;
        }
    }
    strpiped.push_back(tmp);
    
    return strpiped.size();
}

// function for parsing command words
void parseSpace(string& input, vector <string> &parsed){
    	
    string tmp = input;
    size_t pos = tmp.find(" ");
    while(pos!= string::npos){
	parsed.push_back(tmp.substr(0,pos));
	tmp = tmp.substr(pos+1);
	pos = tmp.find(" ");
    }
}

int processString(string &str, vector <string> parsed[MAXLIST]){
    vector <string> strpiped;
    int piped = 0;

    piped = parsePipe(str, strpiped);
         
    for (int i = 0; i < piped; ++i) {
        parseSpace(strpiped.at(i), parsed[i]);
    }

    return 1 + piped;
}

int main(){
    vector <string> parsedArgs[MAXLIST];
    string inputString;
    int execFlag = 0;
    int n=0;
    if(Init()){
	    printf("Init error\n");
	    exit(0);
    }

    while (true) {
        // print shell line
        printf("%% ");
        // take input
        if (takeInput(inputString))
            continue;
        // process
        execFlag = processString(inputString, parsedArgs);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs[0]);

        if (execFlag > 1)
            execArgsPiped(parsedArgs, execFlag);
    }
}
