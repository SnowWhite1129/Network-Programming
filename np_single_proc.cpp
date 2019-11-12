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
#include "nppipe.h"

using namespace std;

User users[max_clients];
Pipe pipe_table[max_clients][max_clients];

int takeInput(int clientID){
    string line;
    Symbol symbol = normal;
    if (!getline(cin, line)){
        puts("");
        exit(0);
    }

    string tmp = "";
    for(int i = 0; i < line.length(); i++)
    {
        fflush(stdout);
        if(line[i] != '\r'){
            tmp += line[i];
        }
    }
    line = tmp;

    if (line.length()==0)
        return false;

    istringstream iss(line);
    string str;

    vector <string> args;
    Pipe stdpipe, ID;
    stdpipe.readfd = STDIN_FILENO;
    stdpipe.writefd = STDOUT_FILENO;
    ID.readfd = -1;
    ID.writefd = -1;
    int receiverID, senderID;

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
            if (str.length()>1){
                symbol = userpipe;
                receiverID = stoi(str.substr(1))-1;
                ID.writefd = receiverID;
            } else{
                symbol = redirectout;
            }
            continue;
        } else if (str[0] == '<'){
            if (symbol != redirectout)
                symbol = normal;
            senderID = stoi(str.substr(1))-1;
            ID.readfd = senderID;
            stdpipe.readfd = pipe_table[senderID][clientID].readfd;
            stdpipe.writefd = pipe_table[senderID][clientID].writefd;
            continue;
        } else {
            args.push_back(str);
            if (symbol != redirectout)
                symbol = normal;
            continue;
        }
        execArgsPiped(args, symbol, clientID, stdpipe, ID, line);
        Pop(users[clientID].cmd);
        args.clear();
    }

    if (symbol == normal || symbol == redirectout){
        execArgs(args, symbol, clientID, stdpipe, ID, line);
        Pop(users[clientID].cmd);
    } else if (symbol == userpipe){
        execArgsPiped(args, symbol, clientID, stdpipe, ID, line);
        Pop(users[clientID].cmd);
    }
    return true;
}

bool Init(){
    clearenv();
    for (int i = 0; i < max_clients; ++i) {
        users[i].ID = -1;
        users[i].fd = -1;
        for (int j = 0; j < max_clients; ++j) {
            pipe_table[i][j].readfd = -1;
            pipe_table[i][j].writefd = -1;
        }
        for (int j = 0; j < MAXLIST; ++j) {
            users[i].cmd[j].Clean();
        }
        users[i].environment["PATH"] = "bin:.";
    }
    return setenv("PATH", "bin:.", true)!=-1;
}

// Function where the system command is executed
bool execArgs(vector <string> &parsed, Symbol symbol, int clientID, Pipe stdpipe, Pipe ID, string line){
    if (parsed.at(0)=="exit"){
        logout(clientID, users);
        closePipe(clientID, pipe_table);
        close(users[clientID].fd);
        users[clientID].Delete();
        return true;
    } else if (parsed.at(0)== "setenv"){
        if(setenv(parsed.at(1).c_str(), parsed.at(2).c_str(), true)==-1){
            printf("Erro setenv");
            return false;
        }
        users[clientID].environment[parsed.at(1)] = parsed.at(2);
        return true;
    } else if(parsed.at(0) == "printenv"){
        printenv(parsed.at(1));
        return true;
    } else if (parsed.at(0) == "yell"){
        yell(clientID, parsed, users);
        return true;
    } else if (parsed.at(0) == "who"){
        who(clientID, users);
        return true;
    } else if (parsed.at(0) == "name"){
        name(clientID, parsed.at(1), users);
        return true;
    } else if (parsed.at(0) == "tell"){
        int sender = stoi(parsed.at(1))-1;
        if (users[sender].ID != -1)
            tell(clientID, stoi(parsed.at(1))-1, parsed, users);
        else
            nouserMessage(sender, users[clientID].fd);
        return true;
    }

    if (ID.readfd != -1){
        if (users[ID.readfd].ID ==-1){
            nouserMessage(ID.readfd, users[clientID].fd);
            return false;
        } else{
            if (checkPipeStatus(ID.readfd, clientID, pipe_table)){
                nomessageMessage(ID.readfd, clientID, users[clientID].fd);
                return false;
            } else{
                recieve(clientID, ID.readfd, line, users);
            }
        }
    }

    // Forking a child
    pid_t pid;
    while((pid=fork())<0){
        usleep(1000);
    }
    if (pid == 0) {
        if (users[clientID].cmd[0].fd[READ_END]!=-1){
            close(users[clientID].cmd[0].fd[WRITE_END]);
            dup2(users[clientID].cmd[0].fd[READ_END], STDIN_FILENO);
            close(users[clientID].cmd[0].fd[READ_END]);
        }
        if (ID.readfd != -1) {
            close(stdpipe.writefd);
            dup2(stdpipe.readfd, STDIN_FILENO);
            close(stdpipe.readfd);
        }

        if (symbol == redirectout){
            int out = open(parsed.at(parsed.size()-1).c_str(), O_WRONLY|O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
            if (out == -1){
                cout << "File open error." << endl;
                return false;
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
        if (users[clientID].cmd[0].fd[READ_END]!=-1){
            close(users[clientID].cmd[0].fd[WRITE_END]);
            close(users[clientID].cmd[0].fd[READ_END]);
        }
        if (ID.readfd != -1){
            pipe_table[ID.readfd][clientID].readfd = -1;
            pipe_table[ID.readfd][clientID].writefd = -1;
            close(stdpipe.readfd);
            close(stdpipe.writefd);
        }
        int status;
        waitpid(pid, &status, 0);
        return true;
    }
}

// Function where the piped system commands is executed
bool execArgsPiped(vector <string> &parsed, Symbol symbol, int clientID, Pipe stdpipe, Pipe ID, string line)
{
    int fd[2], n=1;
    pid_t pid;

    if (symbol != piped && symbol != userpipe)
        n = stoi(parsed.at(parsed.size()-1));

    int prevfd = check(users[clientID].cmd, n);

    if (prevfd == -1) {
        if (pipe(fd) < 0) {
            cout << "Pipe could not be initialized" << endl;
            return false;
        }
        users[clientID].cmd[n].Init(fd);
    } else {
        for (int i = 0; i < 2; ++i)
            fd[i] = users[clientID].cmd[n].fd[i];
    }

    if (symbol == userpipe){
        if (users[ID.writefd].ID == -1){
            nouserMessage(ID.writefd, users[clientID].fd);
            return false;
        } else{
            if (checkPipeExist(clientID, ID.writefd, pipe_table)){
                occuipiedMessage(clientID, ID.writefd, clientID);
                return false;
            } else{
                pipe_table[clientID][ID.writefd].readfd = fd[READ_END];
                pipe_table[clientID][ID.writefd].writefd = fd[WRITE_END];
                send(clientID, ID.writefd, line, users);
            }
        }
        if (ID.readfd != -1){
            if (users[ID.readfd].ID ==-1){
                nouserMessage(ID.readfd, users[clientID].fd);
                return false;
            } else{
                if (checkPipeStatus(ID.readfd, clientID, pipe_table)){
                    nomessageMessage(ID.readfd, clientID, users[clientID].fd);
                    return false;
                } else{
                    recieve(clientID, ID.readfd, line, users);
                }
            }
        }
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
        if (users[clientID].cmd[0].fd[READ_END]!=-1){
            close(users[clientID].cmd[0].fd[WRITE_END]);
            dup2(users[clientID].cmd[0].fd[READ_END], STDIN_FILENO);
            close(users[clientID].cmd[0].fd[READ_END]);
        }
        if (ID.readfd != -1) {
            close(stdpipe.writefd);
            dup2(stdpipe.readfd, STDIN_FILENO);
            close(stdpipe.readfd);
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
        if (users[clientID].cmd[0].fd[READ_END]!=-1){
            close(users[clientID].cmd[0].fd[WRITE_END]);
            close(users[clientID].cmd[0].fd[READ_END]);
        }
        if (ID.readfd != -1){
            pipe_table[ID.readfd][clientID].readfd = -1;
            pipe_table[ID.readfd][clientID].writefd = -1;
            close(stdpipe.readfd);
            close(stdpipe.writefd);
        }
        return true;
    }
}

void chat(const struct sockaddr_in &client, int clientID)
{
    clearenv();
    users[clientID].Set();
    takeInput(clientID);
    write(users[clientID].fd, "% ", strlen("% ")) ;
}
int main(int argc, char *argv[]){
    signal(SIGCHLD, childHandler);

    fd_set readfds;
    int max_sd, sd;

    if(!Init()){
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

    if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0){
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

            welcomeMessage(users[newclient].fd);

            loginMessage(users[newclient].IP.c_str(), users[newclient].port, users[newclient].fd);
            write(users[newclient].fd, "% ", strlen("% "));
            login(newclient, users);
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++)
        {
            sd = users[i].fd;

            if (FD_ISSET(sd , &readfds)>0)
            {
                int saved_stdin = dup(0);
                int saved_stdout = dup(1);
                int saved_stderr = dup(2);
                dup2(sd, STDIN_FILENO);
                dup2(sd, STDOUT_FILENO);
                dup2(sd, STDERR_FILENO);
                chat(cli, i);
                dup2(saved_stdin, STDIN_FILENO);
                dup2(saved_stdout, STDOUT_FILENO);
                dup2(saved_stderr, STDERR_FILENO);
                close(saved_stdin);
                close(saved_stdout);
                close(saved_stderr);
            }
        }
    }
}
