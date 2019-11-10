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
#include <arpa/inet.h>
#include "command.h"
#include "np_single_proc.h"
#include "message.h"

using namespace std;


command cmd[MAXLIST];

User users[max_clients];

void login(int newclient){
    for (int i = 0; i < max_clients; ++i) {
        if (users[i].fd!=-1){
            dup2(users[i].fd, STDOUT_FILENO);
            loginMessage(users[newclient].IP.c_str(), users[newclient].port);
        }
    }
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
        Pop(cmd);
        args.clear();
    }

    if (symbol == normal || symbol == redirectout){
        execArgs(args, symbol);
        Pop(cmd);
    }
    return true;
}

bool Init(User users[]){
    for (int i = 0; i < 30; ++i) {
        users[i].ID = -1;
        users[i].fd = -1;
    }
    for (auto & i : cmd)
        i.Clean();
    return setenv("PATH", "bin:.", true)!=-1;
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

    int prevfd = check(cmd, n);

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

void chat(const struct sockaddr_in &client)
{
    takeInput();
    cout << "% ";
}
int main(int argc, char *argv[]){
    signal(SIGCHLD, childHandler);


    fd_set readfds;
    int max_sd, sd;

    if(!Init(users)){
        printf("Init error\n");
        exit(0);
    }

    int master_socket, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0) {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));

    // Binding newly created socket to given IP and verification
    if ((bind(master_socket, (sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }

    if(setsockopt(master_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0){
        printf("setsockopt failed\n");
        close(master_socket);
        exit(2);
    }

    // Now server is ready to listen and verification
    if ((listen(master_socket, 5)) != 0) {
        exit(0);
    }

    while(true){
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (int i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = users[i].fd;

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET(sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely

        while (select(max_sd + 1 , &readfds , NULL , NULL , NULL)<0);

        len = sizeof(cli);

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if (User::n == 30)
                continue;

            connfd = accept(master_socket, (struct sockaddr *)&cli, &len);
            while (connfd < 0){
                connfd = accept(master_socket, (struct sockaddr *)&cli, &len);
            }
            User tmp;
            tmp.Init(inet_ntoa(cli.sin_addr), 0, ntohs(cli.sin_port), connfd);

            int newclient = addUser(tmp, users);

            dup2(connfd, STDOUT_FILENO);
            welcomeMessage();

            login(newclient);
            //dup2(connfd, STDOUT_FILENO);
            cout << "% ";
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++)
        {
            sd = users[i].fd;

            if (FD_ISSET(sd , &readfds))
            {
                dup2(sd, STDIN_FILENO);
                dup2(sd, STDOUT_FILENO);
                dup2(sd, STDERR_FILENO);
                chat(cli);
            }
        }
    }
}
