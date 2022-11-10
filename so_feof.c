#include "so_stdio.h"

int so_feof(SO_FILE *stream)
{
    int pos = so_ftell(stream);
    int limit = lseek(stream->so_fd, 0, SEEK_END);
    lseek(stream->so_fd, pos, SEEK_SET);
    if (pos == limit || pos > limit){
        stream->isERR = 999;
        return 1;
    }else {
        stream->isERR = 333; 
        return 0;
    }
}

