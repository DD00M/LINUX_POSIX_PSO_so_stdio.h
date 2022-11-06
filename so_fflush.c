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
    if (stream->off_written != 0)
    {
        int d = write(stream->so_fd, stream->buffer, BUFSIZE);
        if (d < 0){
            perror("fflush failed\n");
            return SO_EOF;
        }
        stream->off_written = 0;
        return 0;
    }else{
        return SO_EOF;
    }
}