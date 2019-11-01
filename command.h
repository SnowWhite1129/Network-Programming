#ifndef NETWORK_PROGRAMMING_COMMAND_H
#define NETWORK_PROGRAMMING_COMMAND_H

#endif //NETWORK_PROGRAMMING_COMMAND_H

#define MAXLIST 1000
#define READ_END 0
#define WRITE_END 1

struct command{
    int fd[2];
    command &operator = (const command &tmp);
    void Init(const int fd1[2]);
    void Clean();
};
void Pop(command cmd[]);
int check(const command cmd[], int n);