#include "so_stdio.h"

int so_fseek(SO_FILE *stream, long offset, int whence){
    if (stream->prev == READprev){
        so_fflush(stream);
    }
    if (stream->prev == WRITEprev){
        so_fflush(stream);
    }

    int pos = so_ftell(stream);
    int limit = lseek(stream->so_fd, 0, SEEK_END);
    ///revin
    lseek(stream->so_fd, pos, SEEK_SET);

    stream->cursor = lseek(stream->so_fd, offset, whence);
    if(stream->cursor < 0)
    {
        stream->isERR = 999;
        return SO_EOF;
    }else if (stream->cursor > limit){
        stream->isERR = 999;
        return SO_EOF;
    }
    return 0;
}