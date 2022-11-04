#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

int so_fclose(SO_FILE *stream)
{
    free(stream);
    if (stream != NULL)
    {
        close(stream->so_fd);
        return SO_EOF;
    }else return 0;
}