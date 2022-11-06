#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "so_stdio.h"

#include <stdio.h>

SO_FILE *so_popen(const char *command, const char *type)
{
    int status;
    int flag = 0;
    int myfd;

    int pipe_fd[2];
    int ret = pipe(pipe_fd);
    printf("%d %d\n", pipe_fd[0], pipe_fd[1]);
    if (ret < 0)
    {
        perror("error at creating pipe\n");
        exit(-1);
    }
    printf("FLAG1\n");
    int pid = fork();
    if (pid == 0)
    {
        if (strcmp(type, "r") == 0)
        {
            printf("FLAG2\n");
            myfd = pipe_fd[1];
            int op = dup2(pipe_fd[1], STDOUT_FILENO);
            if (op < 0)
            {
                perror("err dup2 STDOUT\n");
                exit(-1);
            }
            close(pipe_fd[0]);
            flag = 1;
            execlp("/bin/sh", "sh", "-c", command, (char *)0);
        }
        else if (strcmp(type, "w") == 0)
        {
            myfd = pipe_fd[0];
            int op = dup2(pipe_fd[0], 0);
            if (op < 0)
            {
                perror("err dup2 STDOUT\n");
                exit(-1);
            }
            close(pipe_fd[1]);
            execlp("/bin/sh", "sh", "-c", command, (char *)0);
        }
        else
        {
            return NULL;
        }
    }
    else if (pid > 0)
    {
    }
    if (flag == 1){
        printf("aicisea\n");
        SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
        file->so_fd = myfd;
        file->mode = type;
        file->pid = pid;
        file->is_p = 1;
        return file;
    }
    printf("iese\n");
}