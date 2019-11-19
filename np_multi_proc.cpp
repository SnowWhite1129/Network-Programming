
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
#include "np_multi_proc.h"
#include "multimessage.h"
#include "multinppipe.h"
#include "sharememory.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int n=0;
command cmd[MAXLIST];

using namespace std;

ShareMemory *shm = (ShareMemory *)mmap(0, sizeof(ShareMemory), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);

void messageHandler(int signo){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].pid == getpid()){
            for (int j = 0; j < max_clients; ++j) {
                if (strlen(shm->message[j][i]) > 0 ){
                    //TODO: init : clear message
                    if (shm->users[j].ID != -1){
                        cout << shm->message[j][i];
                        fflush(stdout);
                    }
                    strcpy(shm->message[j][i], "");
                }
            }
        }
    }
}
void fifoHandler(int signo){
    for (int i = 0; i < max_clients; ++i) {
        if (shm->users[i].pid == getpid()){
            for (int j = 0; j < max_clients; ++j) {
                if (shm->pipe_status[j][i] && shm->pipe_fd[j][i]==-1){
                    //TODO: init pipe_fd
                    char filepath[1024];
                    sprintf(filepath, "user_pipe/%d_%d", j, i);
                    mkfifo(filepath, 0666);
                    int fifofd = open(filepath, O_RDONLY);
                    shm->pipe_fd[j][i] = fifofd;
                    unlink(filepath);
                }
            }
        }
    }
}
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

    int senderID = -1, receiverID = -1;

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
            } else{
                symbol = redirectout;
            }
            continue;
        } else if (str[0] == '<'){
            if (symbol != redirectout && symbol != userpipe)
                symbol = normal;
            senderID = stoi(str.substr(1))-1;
            continue;
        } else {
            args.push_back(str);
            if (symbol != redirectout)
                symbol = normal;
            continue;
        }
        execArgsPiped(args, symbol, clientID, senderID, receiverID, line);
        Pop(cmd);
        args.clear();
        senderID = -1;
        receiverID = -1;
    }

    if (symbol == normal || symbol == redirectout){
        execArgs(args, symbol, clientID, senderID, line);
        Pop(cmd);
    } else if (symbol == userpipe){
        execArgsPiped(args, symbol, clientID, senderID, receiverID, line);
        Pop(cmd);
    }
    return true;
}

bool Init(){
    clearenv();
    shm->userstatus = false;
    for (int i = 0; i < max_clients; ++i) {
        shm->users[i].Delete();
        for (int j = 0; j < max_clients; ++j) {
            shm->pipe_status[i][j] = false;
            shm->pipe_fd[i][j] = -1;
            strcpy(shm->message[j][i], "");
        }
    }
    for (int i = 0; i < MAXLIST; ++i) {
       cmd[i].Clean();
    }
    return setenv("PATH", "bin:.", true)!=-1;
}

// Function where the system command is executed
void execArgs(vector <string> &parsed, Symbol symbol, int clientID, int sender, const string &line){
    if (parsed.at(0)=="exit"){
        logout(clientID, shm);
        shm->users[clientID].Delete();
        for (int i = 0; i < max_clients; ++i) {
            strcpy(shm->message[i][clientID], "");
        }
        --n;
        exit(0);
    } else if (parsed.at(0)== "setenv"){
        if(setenv(parsed.at(1).c_str(), parsed.at(2).c_str(), true)==-1){
            exit(0);
        }
        return;
    } else if(parsed.at(0) == "printenv"){
        printenv(parsed.at(1));
        return;
    } else if (parsed.at(0) == "yell"){
        string message = line.substr(line.find(' ')+1);
        yell(clientID, shm, message.c_str());
        return;
    } else if (parsed.at(0) == "who"){
        who(clientID, shm->users);
        return;
    } else if (parsed.at(0) == "name"){
        name(clientID, parsed.at(1), shm);
        return;
    } else if (parsed.at(0) == "tell"){
        int ID = stoi(parsed.at(1))-1;
        tell(clientID, ID, line.c_str(), shm);
        return;
    }
    int devNull = -1;

    if (sender != -1) {
        if (!checkPipeStatusMulti(sender, clientID, shm->pipe_status)){
            nomessageMessage(sender, clientID);
            devNull = open("/dev/null", O_RDONLY);
        } else{
            recieve(clientID, sender, line, shm);
        }
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
        } else if (sender != -1){
            if (devNull == -1){
                dup2(shm->pipe_fd[sender][clientID], STDIN_FILENO);
                close(shm->pipe_fd[sender][clientID]);
                shm->pipe_status[sender][clientID] = false;
            } else{
                //TODO: something wrong?
                dup2(devNull, STDIN_FILENO);
                close(devNull);
            }
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
        if (sender != -1){
            close(shm->pipe_fd[sender][clientID]);
            while(shm->pipe_status[sender][clientID]){
                usleep(1000);
            }
            shm->pipe_fd[sender][clientID] = -1;
        }
        int status;
        waitpid(pid, &status, 0);
    }
}

// Function where the piped system commands is executed
void execArgsPiped(vector <string> &parsed, Symbol symbol, int clientID, int sender, int receiver, const string &line)
{
    int fd[2], n=1, fifofd=-1, devNull=-1;
    pid_t pid;

    if (symbol == userpipe){
        if (shm->users[receiver].ID == -1){
            nouserMessage(receiver);
            if (cmd[0].fd[READ_END]!=-1){
                close(cmd[0].fd[WRITE_END]);
                close(cmd[0].fd[READ_END]);
            }
            return;
        } else{
            if (checkPipeStatusMulti(clientID, receiver, shm->pipe_status)){
                //TODO problem
                occuipiedMessage(clientID, receiver);
                if (cmd[0].fd[READ_END]!=-1){
                    close(cmd[0].fd[WRITE_END]);
                    close(cmd[0].fd[READ_END]);
                }
                return;
            }
        }
    }
    if (sender != -1) {
        if (shm->users[sender].ID == -1) {
            nouserMessage(sender);
            return;
        }
    }

    if (symbol != userpipe){
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
    }

    if (sender != -1) {
        if (!checkPipeStatusMulti(sender, clientID, shm->pipe_status)){
            nomessageMessage(sender, clientID);
            devNull = open("/dev/null", O_RDONLY);
        } else{
            recieve(clientID, sender, line, shm);
        }
    }

    char filepath[1024];

    if (symbol == userpipe){
        send(clientID, receiver, line, shm);
        sprintf(filepath, "user_pipe/%d_%d", clientID, receiver);
        mkfifo(filepath, 0666);
        shm->pipe_status[clientID][receiver] = true;
        kill(shm->users[receiver].pid, SIGUSR1);
        fifofd = open(filepath, O_WRONLY);
    }

    while((pid=fork())<0){
        usleep(1000);
    }

    if (pid==0){
        if (fifofd != -1){
            dup2(fifofd, STDOUT_FILENO);
            close(fifofd);
        } else{
            close(fd[READ_END]);
            dup2(fd[WRITE_END], STDOUT_FILENO);
        }
        if(symbol == numberexplamation)
            dup2(fd[WRITE_END], STDERR_FILENO);
        if (symbol != userpipe)
            close(fd[WRITE_END]);
        if (cmd[0].fd[READ_END]!=-1){
            close(cmd[0].fd[WRITE_END]);
            dup2(cmd[0].fd[READ_END], STDIN_FILENO);
            close(cmd[0].fd[READ_END]);
        } else if (sender != -1) {
            if (devNull == -1){
                dup2(shm->pipe_fd[sender][clientID], STDIN_FILENO);
                close(shm->pipe_fd[sender][clientID]);
                shm->pipe_status[sender][clientID] = false;
            } else{
                //TODO: something wrong?
                dup2(devNull, STDIN_FILENO);
                close(devNull);
            }
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
        if (fifofd != -1){
            close(fifofd);
        }
        if (sender != -1){
            close(shm->pipe_fd[sender][clientID]);
            while(shm->pipe_status[sender][clientID]){
                usleep(1000);
            }
            shm->pipe_fd[sender][clientID] = -1;
        }
        if (devNull != -1)
            close(devNull);
    }
}

void chat(int clientID)
{
    // infinite loop for chat
    welcomeMessage();
    login(clientID, shm);
    while (true) {
        printf("%% ");
        if (!takeInput(clientID))
            continue;
    }
}
int main(int argc, char *argv[]){
    signal(SIGCHLD, childHandler);
    signal(SIGINT, exitHandler);
    signal(SIGUSR1, fifoHandler);
    signal(SIGUSR2, messageHandler);

    if(!Init()){
        printf("Init error\n");
        exit(0);
    }

    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }

    if (setsockopt(sockfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (struct sockaddr *)&servaddr , sizeof(servaddr)) < 0){
        printf("setsockopt failed\n");
        close(sockfd);
        exit(2);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        exit(0);
    }

    while(true){
        len = sizeof(cli);

        if ( n > 30){
            continue;
        }

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
        if (connfd < 0) {
            exit(0);
        }

        pid_t pid;
        while((pid=fork())<0){
            usleep(1000);
        }
        if (pid == 0){
            close(sockfd);
            // Function for chatting between client and server
            for (int i = 0; i < 3; ++i) {
                close(i);
            }

            User tmp;
            tmp.Init(inet_ntoa(cli.sin_addr), 0, ntohs(cli.sin_port) , getpid());
            int clientID = addUser(tmp, shm->users);

            dup2(connfd, STDIN_FILENO);
            dup2(connfd, STDOUT_FILENO);
            dup2(connfd, STDERR_FILENO);
            close(connfd);

            chat(clientID);
            exit(0);
        }
        close(connfd);
    }
}
