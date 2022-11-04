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
    //write(1, "FLAG\n", 6);
    if (stream->buffer_index < BUFSIZE){
        //write(1, "FLAG1\n", 6);
        //printf("%d %d\n", stream->buffer_index, BUFSIZE);
        //printf("%d\n", stream->buffer_index);
        //printf("CARACTER: %c", c);
        stream->buffer[stream->buffer_index] = (char)c;
        //printf("%c\n", c);
        //printf("%c\n", stream->buffer[stream->buffer_index]);
        //printf("%s\n", stream->buffer);
        //write(1, "A", 2);
        //write(1, "FLAG2\n", 6);
        
        stream->buffer_index += 1;
        stream->off_written += 1;
        stream->cursor += 1;
        return c;
    }else{
        //write(1, "FLAG3\n", 6);
        stream->buffer[BUFSIZE] = '\0';
          //printf("%d %d\n", stream->buffer_index, BUFSIZE);
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