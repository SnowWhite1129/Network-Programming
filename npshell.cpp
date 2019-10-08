#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>
#include<iostream>
using namespace std;

#define MAXCOM 256
#define MAXLIST 256
#define MAXBUFFERSIZE 15000
#define READ_END 0
#define WRITE_END 1

// Function to take input 

struct command{
	int n = 1;
	int fd = -1;
};


int takeInput(char* str){
    char* buf = (char *)malloc(sizeof(char)* MAXBUFFERSIZE);

    if(!buf){
    	cout << "Malloc error." << endl;
    	return 0;
    }
    buf = fgets(buf, MAXBUFFERSIZE, stdin);
    int len = strlen(buf);
    if (len > 1) {
        buf[len-1] = '\0';
        strcpy(str, buf);
        free(buf);
        return 0;
    } else {
    	free(buf);
        return 1;
    }
}

void printenv(const char name[]){
	cout << getenv(name) << endl;
}
bool Init(){
	return setenv("PATH", "bin:.", true);
}

// Function where the system command is executed 
void execArgs(char** parsed)
{
    if(strcmp(parsed[0], "exit")==0){
        exit(0);
    }else if (strcmp(parsed[0], "setenv")==0){
	if(!setenv(parsed[1], parsed[2], true)){
		printf("Set env Error\n");
		exit(0);	
	}
	return;

    }else if(strcmp(parsed[0], "printenv")==0){
    	printenv(parsed[1]);
	return;
    }
    // Forking a child 
    pid_t pid = fork();

    if (pid == -1) {
        cout << "Failed forking child" << endl ;
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("Unknown command: [%s].\n", parsed[0]);
	    
        }
        exit(0);
    } else {
        // waiting for child to terminate 
        wait(0);
        return;
    }
}

// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe, int pipe_count)
{
    // 0 is read end, 1 is write end 
    int fd[pipe_count][2], status;
    pid_t p1, p2;

        for(int i = 0;i < pipe_count;i++){
            if (pipe(fd[i]) < 0) {
        	printf("Pipe could not be initialized\n");
        	return;
	    }
        }

        for(int i = 0;i <= pipe_count;i++){
            p1 = fork();
	    if (p1 < 0) {
        	printf("Could not fork\n");
        	return;
	    }

            if(!p1){
                if(i!=0){
                    dup2(fd[i-1][0],0);
                }

                if(i!=pipe_count){
                    dup2(fd[i][1],1);
                }

                for(int j = 0;j < pipe_count;j++){   
                    close(fd[j][0]);
                    close(fd[j][1]);
                }

                if (execvp(parsed[0], parsed) < 0) {
            	    printf("Could not execute [%s].\n", parsed[0]);
        	    exit(0);
	        }
                exit(0);
            }
        }
        for(int i = 0;i < pipe_count;i++){
            close(fd[i][0]);
            close(fd[i][1]);
        }
        waitpid(p1,&status,0);          
}

// It's assumption is only contain one pipe
// function for finding pipe 
int parsePipe(char* str, char** strpiped)
{
    int i=0;
    for(int j=0;str[j]!='\0' && i < MAXLIST;++j){
	switch(str[j]){
	case '|':
		if(isdigit(str[j+1]))
			cout << "number piped" << endl;//numberpipe	
		strpiped[i] = strsep(&str, "|");
		++i;
		break;
	case '!':
		strpiped[i] = strsep(&str, "!");
		++i;
		break;
	case '>':
		strpiped[i] = strsep(&str, ">");
		++i;
		break;
	}
    }	
    
    for(int k=0; strpiped[k]!=0; ++k)
	cout << strpiped[k] << endl;	

    return i;   
}

// function for parsing command words 
void parseSpace(char* str, char** parsed){
    int i;

    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
	
        if (parsed[i] == 0)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char* str, char** parsed, char** parsedpipe){
    char* strpiped[MAXLIST];
    int piped = 0;

    piped = parsePipe(str, strpiped);

    if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);

    } else {
        parseSpace(str, parsed);
    }

    return 1 + piped;
}

int main(){
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    command c[1000];
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
        execFlag = processString(inputString,
                                 parsedArgs, parsedArgsPiped);
        // execflag returns zero if there is no command 
        // or it is a builtin command, 
        // 1 if it is a simple command 
        // 2 if it is including a pipe. 

        // execute 
        if (execFlag == 1)
            execArgs(parsedArgs);

        if (execFlag > 1)
            execArgsPiped(parsedArgs, parsedArgsPiped, execFlag);
    }
    exit(0);
} 
