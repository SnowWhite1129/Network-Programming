#include <utility>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <iostream>
#include <fcntl.h>
#include <netinet/in.h>
#include "npshell.h"

#define MAXLIST 1000
#define READ_END 0
#define WRITE_END 1

using namespace std;

command cmd[MAXLIST];

void User::Init(string IP1, int ID1, int port1) {
    ID = ID1;
    port = port1;
    IP = move(IP1);
    name = "(no name)";
    ++n;
}
void User::Delete() {
    ID = -1;
    --n;
}

void welcomeMessage(){
    cout << "***************************************\n"
            "** Welcome to the information server **\n"
            "***************************************" << endl;
}
void loginMessage(const char IP[], int port){
    cout << "*** User '(no name)' entered from " << IP << ":" << port << ". ***" << endl;
}
void logoutMessage(string name){
    cout << "*** User '(no name)' left. ***" << endl;
}
void yellMessage(const char name[], const char message[]){
    cout << "*** " << name << " yelled ***: " << message << endl;
}
void toldMessage(const char name[], const char message[]){
    cout << "*** " << name << " told you ***: " << message << endl;
}
void sendMessage(const char sendername[], int senderID, const char message[], const char receivername[], int receiverID){
    cout << "*** " << sendername << " " << "(#" << senderID <<  ") just piped '" << message <<" ' to " << receivername <<" (#" << receiverID << ") ***" << endl;
}
void receiveMessage(const char receivername[], int receiverID, const char message[], const char sendername[], int senderID){
    cout << "*** " << receivername << " " << "(#" << receiverID <<  ") just received from '" << receivername <<" (#" << receiverID << ") " << "by '" << message << "' ***" << endl;
}
void nouserMessage(int ID){
    cout << "*** Error: user #" << ID << " does not exist yet. ***";
}
void nomessageMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " does not exist yet. ***" << endl;
}
void occuipiedMessage(int senderID, int receiverID){
    cout << "*** Error: the pipe #" << senderID << "->#" << receiverID << " already exists. ***" << endl;
}
void command::Init(const int fd1[2]) {
    for (int i = 0; i < 2; ++i) {
        fd[i] = fd1[i];
    }
}
command& command::operator=(const command &tmp) {
    for (int i = 0; i < 2; ++i) {
        fd[i] = tmp.fd[i];
    }
}
void command::Clean() {
    for (int & i : fd) {
        i = -1;
    }
}

void childHandler(int signo){
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
    // NON-BLOCKING WAIT
    // Return immediately if no child has exited.
}

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
    if (!getline(cin, line)){
        puts("");
        exit(0);
    }

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

bool Init(User users[]){
    for (int i = 0; i < 30; ++i) {
        users[i].ID = -1;
    }
    User::n = 0;
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
            exit(0);
        }
        return;
    } else if(parsed.at(0) == "printenv"){
    	printenv(parsed.at(1));
	    return;
    }

    // Forking a child
    pid_t pid;
    while((pid=fork())<0){
        usleep(1000);
    }
    if (pid == 0) {
	    if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            dup2(cmd[0].fd[READ_END], STDIN_FILENO);
            close(cmd[0].fd[READ_END]);
	    }

        if (symbol == redirectout){
            int out = open(parsed.at(parsed.size()-1).c_str(), O_WRONLY|O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
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
            fprintf(stderr, "Unknown command: [%s].\n", args[0] );
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

    while((pid=fork())<0){
        usleep(1000);
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
            fprintf(stderr, "Unknown command: [%s].\n", args[0]);
        argsFree(args);
        exit(0);
    } else {
        if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            close(cmd[0].fd[READ_END]);
        }
    }
}
