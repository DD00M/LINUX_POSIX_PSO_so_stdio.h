#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

#include <stdio.h>

SO_FILE *so_popen(const char *command, const char *type)
{
    printf("aici odtx\n");
    int pipe_fd[2];
    int ret = pipe(pipe_fd);
    printf("%d %d\n", pipe_fd[0], pipe_fd[1]);
    int op = dup2(pipe_fd[1], 1);
    printf("%d\n", op);
    if (op < 0){
        perror("err dup2 STDOUT\n");
        exit(-1);
    }
    if (ret < 0)
    {
        perror("error at creating pipe\n");
        exit(-1);
    }
    printf("aici odt2\n");
    int pid = fork();
    printf("aici odt\n");
    if (pid == 0)
    {
        if (strcmp(type , "r") == 0)
        {
            printf("ceva\n");
            close(pipe_fd[0]);
            execl("/bin/sh", "sh", "-c", command, (char *)0);
            SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
            if (file == NULL)
            {
                perror("error in opening Pfile\n");
                exit(-1);
            }
            file->so_fd = pipe_fd[0];
            file->mode = 'r';
            return file;
        }
        else if (strcmp(type , "w") == 0)
        {
            close(pipe_fd[0]);
            execl("/bin/sh", "sh", "-c", command, (char *)0);
            SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
            if (file == NULL)
            {
                perror("error in opening Pfile\n");
                exit(-1);
            }
            file->so_fd = pipe_fd[1];
            file->mode = 'w';
            return file;
        }
        else
            return NULL;
    }
    else if (pid > 0)
    {
        waitpid(pid);
    }
    else if (pid < 0)
    {
        perror("Bad pid\n");
        exit(-1);
    }
}