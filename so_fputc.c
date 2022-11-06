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

    if (stream->buffer_index < BUFSIZE){
        stream->buffer[stream->buffer_index] = (char)c;
        stream->buffer_index += 1;
        stream->off_written += 1;
        stream->cursor += 1;
        return c;
    }else{
        stream->buffer[BUFSIZE] = '\0';
        int check = write(stream->so_fd, stream->buffer, BUFSIZE);
        if (check <= 0){
            return SO_EOF;
        }
        stream->buffer_index = 0;
        stream->off_written += BUFSIZE;
        stream->cursor += 1;
        return c;
    }
}