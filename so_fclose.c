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
    if (stream == NULL){
        return SO_EOF;
    }

    if (stream->is_file == 0){
        return SO_EOF;
    }

    so_fflush(stream);
    int k = close(stream->so_fd);
    if (k == -1)
    {
        free(stream);
        return SO_EOF;
    }
    free(stream);
    return 0;
}
