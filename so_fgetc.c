#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

#include <stdio.h>

int so_fgetc(SO_FILE *stream)
{
    if (strcmp(stream->mode, "w") == 0){
        return SO_EOF;
    }
    if (stream->firstIndex == stream->lastIndex)
    {
        ssize_t bytesRead = read(stream->so_fd, stream->buffer, BUFSIZE);
        stream->lastIndex = (int)bytesRead - 1;
        stream->firstIndex = 0;
        stream->buffer_index = 0;
        if (stream->lastIndex == -1){
            return NULL;
        }
        if (bytesRead <= 0)
        {
            return SO_EOF;
        }
        else
        {
            stream->buffer_index = 0;
            stream->off_read = bytesRead;
            stream->cursor += 1;
            return (int)stream->buffer[0];
        }
    }
    else
    {
        stream->firstIndex += 1;
        stream->buffer_index += 1;
        stream->cursor += 1;
        return (int)stream->buffer[stream->firstIndex];
    }
}