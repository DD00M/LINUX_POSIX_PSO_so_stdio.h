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
    int *pipe_fd = (int*)malloc(2*sizeof(int));
    if (pipe_fd == NULL){
        printf("bad alloc\n");
        return NULL;
    }
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
        printf("FLAG2\n");
        int op = dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        if (op < 0)
        {
            perror("err dup2 STDOUT\n");
            exit(-1);
        }
        if (strcmp(type, "r") == 0)
        {
            execl("/bin/sh", "sh", "-c", command, (char *)0);
            SO_FILE *file = (SO_FILE *)malloc(sizeof(SO_FILE));
            if (file == NULL)
            {
                perror("error in opening Pfile\n");
                exit(-1);
            }
            file->so_fd = pipe_fd[0];
            file->mode = "r";
            close(pipe_fd[1]);
            return file;
        }
        else if (strcmp(type, "w") == 0)
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
            file->mode = "w";
            return file;
        }
        else
        {
            return NULL;
        }
    }
    else if (pid > 0)
    {
        waitpid(pid, &status, 0);
    }
    return NULL;
}

int main(){
    SO_FILE *lala = so_popen("ls", "r");
    return 0;
}