#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

#include <stdio.h>

int so_fputc(int c, SO_FILE *stream)
{
    if (strcmp(stream->mode, "r") == 0){
        return SO_EOF;
    }
    if (strcmp(stream->mode, "a") == 0 || strcmp(stream->mode, "a+") == 0){
        lseek(stream->so_fd, 0, SEEK_END);
    }
    if (stream->prev == READprev){
        stream->buffer_index = 0;
        stream->off_written = 0;
        memset(stream->buffer, 0, BUFSIZE);
    }

    stream->prev = WRITEprev;

    if (stream->off_written == BUFSIZE){
        so_fflush(stream);
    }

    stream->buffer[stream->buffer_index] = (int)c;

    stream->off_written+=1;
    stream->buffer_index+=1;   
    stream->cursor+=1; 
    return c;
}