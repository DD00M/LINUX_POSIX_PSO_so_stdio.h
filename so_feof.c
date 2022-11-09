#include "so_stdio.h"

int so_feof(SO_FILE *stream)
{
    int pos = so_ftell(stream);
    int limit = lseek(stream->so_fd, 0, SEEK_END);
    ///revin
    lseek(stream->so_fd, pos, SEEK_SET);
    if (pos == limit){
        return 0;
        stream->isERR = 888;
    }else {return 1; stream->isERR = 333; }
}
