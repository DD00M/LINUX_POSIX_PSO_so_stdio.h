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
    if (stream->firstIndex == stream->lastIndex)
    {
        // start:
        // write(1, "flag1\n", 6);
        ssize_t bytesRead = read(stream->so_fd, stream->buffer, BUFSIZE);
        stream->lastIndex = (int)bytesRead - 1;
        printf("%d %d\n", stream->firstIndex, stream->lastIndex);
        stream->firstIndex = 0;
        stream->buffer_index = 0;
        printf("pe egal\n");
        // printf("%d", stream->so_fd);
        if (bytesRead <= 0)
        {
            // write(1, "flag2\n", 6);
            return SO_EOF;
        }
        else
        {
            // write(1, "flag3\n", 6);
            stream->buffer_index = 0;
            stream->off_read = bytesRead;
            stream->cursor += 1;
            return (int)stream->buffer[0];
        }
    }
    else
    {
        // write(1, "flag4\n", 6);
        printf("pe pizda ma tii\n");
        printf("%d %d\n", stream->firstIndex, stream->lastIndex);
        stream->firstIndex += 1;
        stream->buffer_index += 1;
        stream->cursor += 1;
        return (int)stream->buffer[stream->firstIndex];
    }
}