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

#define READ_END 0
#define WRITE_END 1

SO_FILE *so_popen(const char *command, const char *type)
{
    int status;
    int myfd;

    int pipe_fd[2];
    int ret = pipe(pipe_fd);
    //printf("%d %d\n", pipe_fd[READ_END], pipe_fd[WRITE_END]);
    if (ret < 0)
    {
        perror("error at creating pipe\n");
        exit(-1);
    }
    //printf("FLAG1\n");

     if (strcmp(type, "r") == 0){
        myfd = pipe_fd[READ_END];
    }else if (strcmp(type, "w") == 0){
        myfd = pipe_fd[WRITE_END];
    }


    int pid = fork();  
    if (pid == 0)
    {
        if (strcmp(type, "r") == 0)
        {
            
            int op = dup2(pipe_fd[WRITE_END], 1);
            if (op < 0)
            {
                perror("err dup2 STDOUT\n");
                exit(-1);
            }
            close(pipe_fd[READ_END]);
            
            execlp(command, NULL);
        }
        else if (strcmp(type, "w") == 0)
        {
            int op = dup2(pipe_fd[WRITE_END], 1);
            if (op < 0)
            {
                perror("err dup2 STDOUT\n");
                exit(-1);
            }
            close(pipe_fd[READ_END]);
            execlp(command, NULL);
        }
        else
        {
            return NULL;
        }
    }
    else if (pid > 0)
    {
        //printf("A INTRAT AICI\n");
        SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
        file->buffer_index = 0;
        file->off_written = 0;
        file->cursor = 0;
        file->so_fd = myfd;
        strcpy(file->mode, type);
        file->pid = pid;
        file->is_p = 1;
        return file;
    }
}