#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    pid_t  pid = fork();
    printf("hello\n");
    if (pid == 0)
        exit(0);

    sleep(1);
    pid = wait(NULL);
    int pid_2 = wait(NULL);
    printf("exit! %d %d\n", pid, pid_2);
    return 0;
}