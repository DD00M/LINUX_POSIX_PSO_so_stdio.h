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
    printf("a intrat in fflush\n");
    if (stream->off_written != 0)
    {
        //printf("a intrat in if\n");
        //printf("%s\n", stream->buffer);
        //printf("%d\n", stream->off_written);
        write(stream->so_fd, stream->buffer, BUFSIZE);
        stream->off_written = 0;
        return 0;
    }else{
        return SO_EOF;
    }
}