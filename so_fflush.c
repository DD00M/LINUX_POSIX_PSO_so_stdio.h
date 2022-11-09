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
            int d = write(stream->so_fd, stream->buffer, stream->off_written);
            if (d < 0)
            {
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
            stream->isERR = 888;
            return 0;
        }return SO_EOF;
    }
    else
    {
        stream->isERR = 555;
        return SO_EOF;
    }
}