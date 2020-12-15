/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 *
 * 518030910356 DongYiyuan 2020.11
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE        80 /* 80 chars per line, per command */


/**
 * parse the input command line into each args
 * the return value represents whether it's a background process
 * that is, whether contains a "$" in the end (return 1 if contain)
 * the last arg is NULL, "$" will not appear in the args
 * return -1 if parse an empty line
 */
int parse_input(char *input, char *result[]);

/**
 * just execute and wait
 */
void simple_execute(char *args[], int if_background);

/**
 * maybe read the name of the parameter is enough to understand
 */
void redirect(char *args[], char *filename, int if_background, int if_input);

/**
 * args_1 pass data to args_2
 */
void do_pipe(char *args_1[], char *args_2[], int if_background);

/**
 * A wrapper
 * SHOULD NEVER RETURN
 * somebody says every syscall should check its return value
 */
void Execvp(char *args[]) {
    execvp(args[0], args);
    printf("%s: Command not found or something unexpected occurred\n", args[0]);
    exit(-1);
}

/**
 * another wrapper
 */
pid_t Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("Error while fork()!\n");
        exit(-1);
    }
    return pid;
}

int main(void) {
    char *args[MAX_LINE / 2 + 1];    /* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    int if_background;

    char file_name[MAX_LINE + 1];
    char input_buf[MAX_LINE + 1];
    char last_input_buf[MAX_LINE + 1] = {};
    while (should_run) {
        printf("osh>");
        fflush(stdout);
        fgets(input_buf, MAX_LINE, stdin);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */

        if (!strcmp(input_buf, "!!\n")) {
            if (last_input_buf[0] == '\0') {
                printf("No commands in history.\n");
                continue;
            }
            printf("%s", last_input_buf);
            if_background = parse_input(last_input_buf, args);
        }
        else {
            if_background = parse_input(input_buf, args);
            if (if_background < 0)
                continue;
            strcpy(last_input_buf, input_buf);
        }

        if (!strcmp(args[0], "exit")) {
            should_run = 0;
            continue;
        }

        int index = 0;
        int if_execute = 0;
        while (args[index]) {

            if (!strcmp(args[index], "|")) {
                if_execute++;
                args[index] = NULL;
                do_pipe(args, args + index + 1, if_background);
                break;
            }

            if (!strcmp(args[index], "<") || !strcmp(args[index], ">")) {
                if_execute++;
                strcpy(file_name, args[index + 1]);
                int if_input = args[index][0] == '<' ? 1 : 0;

                for (int i = index ; ; ++i){
                    args[i] = args[i + 2];
                    if (args[i + 2] == NULL)
                        break;
                }

                redirect(args, file_name, if_background, if_input);
                break;
            }

            index++;
        }

        if (!if_execute)
            simple_execute(args, if_background);
    }

    return 0;
}

int parse_input(char *input, char *result[]) {
    static char buf[MAX_LINE];
    int argc = 0;
    char *parse_ptr = buf, *next_ptr;
    strcpy(buf, input);
    buf[strlen(buf) - 1] = ' ';
    while (*parse_ptr == ' ')
        ++parse_ptr;
    while ((next_ptr = strchr(parse_ptr, ' '))) {
        result[argc++] = parse_ptr;
        *next_ptr = '\0';
        parse_ptr = next_ptr + 1;
        while (*parse_ptr == ' ')
            parse_ptr++;
    }

    if (argc == 0)
        return -1;

    if (!strcmp(result[argc - 1], "&")) {
        result[argc - 1] = NULL;
        return 1;
    }
    result[argc] = NULL;
    return 0;
}

void simple_execute(char *args[], int if_background) {
    pid_t pid = Fork();
    if (pid == 0) {
        Execvp(args);
    }

    if (!if_background) {
        /**
         * wait() is recommended, but I prefer waitpid()  :)
         * I think if you use wait(), you'd code like:
         *     while (pid != fore_ground_pid)
         *         pid = wait(NULL);
         * Because you should wait THE foreground process, not ANY process
         */
        waitpid(pid, NULL, 0);
    }
}

void redirect(char *args[], char *filename, int if_background, int if_input) {
    pid_t pid = Fork();
    int fd;
    if (pid == 0) {
        if (if_input) {
            fd = open(filename, O_RDONLY);
            dup2(fd, STDIN_FILENO);
        } else {
            /**
             * If you do not set the create mode correctly
             * nobody could access that file (including yourself...)
             */
            fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            dup2(fd, STDOUT_FILENO);
        }

        if (fd < 0) {
            printf("Error while opening the file!\n");
            return;
        }
        execvp(args[0], args);
        printf("%s: Command not found or something unexpected occurred\n", args[0]);
        exit(0);
    }

    if (!if_background) {
        waitpid(pid, NULL, 0);
    }
}


void do_pipe(char *args_1[], char *args_2[], int if_background) {
    int fd[2];
    pipe(fd);
    int fd_write = fd[1], fd_read = fd[0];
    pid_t pid;
    pid = Fork();

    if (pid == 0) {
        close(fd_read);
        dup2(fd_write, STDOUT_FILENO);
        Execvp(args_1);
    }

    pid = Fork();
    if (pid == 0) {
        close(fd_write);
        dup2(fd_read, STDIN_FILENO);
        Execvp(args_2);
    }

    /**
     * you'd better close the unused read fd
     * (for logic, save fd, and correctly discover the miss of the read side)
     *
     * you MUST close all the unused write fd
     * Since the read side will not terminate until detects a EOF
     * But if there exist other writers, there will not be any EOF
     */
    close(fd_write);
    close(fd_read);
    if (!if_background) {
        waitpid(pid, NULL, 0);
    }
}