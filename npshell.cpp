#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<vector>

#define MAXCOM 1000 // max number of letters to be supported 
#define MAXLIST 100 // max number of commands to be supported 
#define MAXBUFFERSIZE 1000

// Function to take input 
int takeInput(char* str){
    char* buf = (char *)malloc(sizeof(char)* MAXBUFFERSIZE);

    if(!buf){
    	printf("Malloc error.\n");
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
	char directory[100];
	getcwd(directory, sizeof(directory));

	char *r = strdup(getenv(name));
	// check for errors
	
	//printf("%s\n", r);
	
	char *tok = r, *end = r;
	while (tok != NULL) {
    		strsep(&end, ":");
		if(strcmp(tok, directory)==0){
			printf(".");
		}else{
			printf("%s %s\n", directory, tok);
			for(int i=strlen(directory);i<strlen(tok);++i){
				printf("%c", tok[i]);
			}
		}
    		tok = end;
	}
	free(r);
}
bool Init(){
	char buf[1000];
	getcwd(buf, sizeof(buf));
	strcat(buf, "/bin");
	return setenv("PATH", buf, true);
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
        printf("Failed forking child..\n");
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
void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end 
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("Pipe could not be initialized\n");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("Could not fork\n");
        return;
    }

    if (p1 == 0) {
        // Child 1 executing.. 
        // It only needs to write at the write end 
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) {
            printf("Could not execute [%s].\n", parsed[0]);
            exit(0);
        }
    } else {
        // Parent executing 
        p2 = fork();

        if (p2 < 0) {
            printf("Could not fork\n");
            return;
        }

        // Child 2 executing.. 
        // It only needs to read at the read end 
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("Could not execute command [%s].\n", parsedpipe[0]);
                exit(0);
            }
        } else {
            // parent executing, waiting for two children 
            wait(0);
            wait(0);
        }
    }
}

// It's assumption is only contain one pipe
// function for finding pipe 
int parsePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == 0)
            break;
    }

    if (strpiped[1] == 0)
        return 0; // returns zero if no pipe is found. 
    else {
        return 1;
    }
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
    char* strpiped[2];
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

        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    exit(0);
} 
