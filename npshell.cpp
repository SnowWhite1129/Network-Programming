#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<iostream>
#include "npshell.h"

#define MAXLIST 1000

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
void execArgsPiped(vector<vector <string> > &parsed, int pipe_count)
{
    // 0 is read end, 1 is write end
    int fd[pipe_count][2], status;
    pid_t p1;
    char *args[MAXLIST];

    for(int i = 0;i < pipe_count;++i){
        if (pipe(fd[i]) < 0) {
            cout << "Pipe could not be initialized" << endl;
            return;
        }
    }

    //cout << parsed.at(0).size() << endl;
    //cout << "HI " << parsed.at(1).at(0) << endl;
    //cout << parsed.at(0).at(1).c_str();

    for(int i = 0;i <= pipe_count;++i){
        p1 = fork();
        if (p1 < 0) {
            cout << "Could not fork" << endl;
            return;
        }

        if (p1==0){
            if(i != 0)
                dup2(fd[i-1][READ_END], STDIN_FILENO);

            if(i != pipe_count)
                dup2(fd[i][WRITE_END], STDOUT_FILENO);

            for(int j = 0;j < pipe_count;j++){
                close(fd[j][READ_END]);
                close(fd[j][WRITE_END]);
            }
            for(int j=0; j<parsed.at(i).size();j++){
                //cout << j;
                args[j] = strdup(parsed.at(i).at(j).c_str());
                //cout << args[j] << endl;
            }

            //cout << parsed.at(i).size() << endl;
            //cout << "HI " << parsed.at(i).at(0) << endl;
            //args[parsed.at(i).size()] = NULL;

            if (execvp(parsed.at(i).at(0).c_str(), args) < 0) {
                cout << "Could not execute [" << parsed.at(i).at(0) << "]." << endl;
                argsFree(args);
            }
            exit(0);
        }
    }
    for(int i = 0;i < pipe_count;i++){
        close(fd[i][READ_END]);
        close(fd[i][WRITE_END]);
    }
    waitpid(p1, &status, 0);
    argsFree(args);
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

        if(pos==0){
            tmp = tmp.substr(1);
            pos = tmp.find(" ");
            continue;
        }

        parsed.push_back(tmp.substr(0,pos));
        tmp = tmp.substr(pos+1);
        pos = tmp.find(" ");
    }
    if (tmp!="")
        parsed.push_back(tmp);

//    for (int i = 0; i < parsed.size(); ++i) {
//        cout << parsed.at(i) << " ";
//    }
//    cout << endl;
}

int processString(string &str, vector< vector <string> > &parsed){
    vector <string> strpiped;
    int piped = 0;

    piped = parsePipe(str, strpiped);

//    for (int i = 0; i < strpiped.size(); ++i) {
//        cout << strpiped.at(i) << endl;
//    }

//    cout << "piped" << piped << endl;

    for (int i = 0; i < piped; ++i) {
	    parsed.push_back(vector<string>());
        parseSpace(strpiped.at(i), parsed.at(i));
    }

//    cout << "Size:" << parsed[0].size() << endl;

    return piped;
}

int main(){
    int execFlag = 0;
    int n=0;
    if(Init()){
	    printf("Init error\n");
	    exit(0);
    }

    while (true) {
	    vector<vector <string> >parsedArgs;
	    string inputString;
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

//        for(int i=0; i<parsedArgs.size();++i){
//            cout << i << endl;
//            for(int j=0;j<parsedArgs.at(i).size();++j){
//                cout << j << endl;
//                cout << parsedArgs.at(i).at(j) << " ";
//            }
//            cout << endl;
//        }

        // execute
        if (execFlag == 1)
            execArgs(parsedArgs[0]);

        if (execFlag > 1)
            execArgsPiped(parsedArgs, execFlag-1);
    }
}