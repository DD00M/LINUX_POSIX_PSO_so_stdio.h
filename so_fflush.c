#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

#include <stdio.h>

int so_fflush(SO_FILE *stream)
{
    if (stream->prev == WRITEprev)
    {
        if (stream->off_written != 0)
        {
            //printf("cucubau: %d %d\n", stream->off_written, stream->so_fd);
            //printf("\n\n\nBUFFFER\n%s\n\n\n\n", stream->buffer);
            int d = write(stream->so_fd, stream->buffer, stream->off_written);
            //printf("\n\n\n\n\ndata: %d %d\n\n\n\n\n", d, stream->off_written);
            if (d < 0)
            {
                printf("a dat fail la write\n");
                stream->isERR = 555;
                return SO_EOF;
            }
            stream->buffer_index = 0;
            stream->off_written = 0;
            // memset(stream->buffer, 0, BUFSIZE);
            for (int i = 0; i < BUFSIZE; i++)
            {
                stream->buffer[i] = '\0';
            }
            stream->isERR = 999;
            return 0;
        }return SO_EOF;
    }
    else
    {
        stream->buffer_index = 0;
        stream->off_written = 0;
            // memset(stream->buffer, 0, BUFSIZE);
        for (int i = 0; i < BUFSIZE; i++)
        {
            stream->buffer[i] = '\0';
        }

        stream->isERR = 555;
        return SO_EOF;
    }
}