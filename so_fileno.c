#include "so_stdio.h"

int so_fileno(SO_FILE *stream)
{
    if (stream == NULL){
        return SO_EOF;
    }
    if (stream->so_fd != -1)
    {
        stream->isERR = 888;
        return stream->so_fd;
    }else {stream->isERR = 555; return -1;};
}