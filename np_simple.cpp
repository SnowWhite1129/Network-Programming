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
#include <dnet.h>
#include "npshell.h"
int main(){
    signal(SIGCHLD, childHandler);

    if(!Init()){
        printf("Init error\n");
        exit(0);
    }

    int sockfd, connfd;
    unsigned int len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("socket creation failed...\n");
        exit(0);
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;s
    servaddr.sin_addr.s_addr = htonl("127.0.0.1");
    servaddr.sin_port = htons(7001);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        exit(0);
    }
    while(true){
        len = sizeof(cli);

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
            dup2(sockfd, STDIN_FILENO);
            dup2(sockfd, STDOUT_FILENO);
            dup2(sockfd, STDERR_FILENO);

            func(connfd);
            exit(0);
        }
        close(connfd);
    }
}
