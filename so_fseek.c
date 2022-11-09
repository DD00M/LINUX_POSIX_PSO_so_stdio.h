#include "so_stdio.h"

int so_fseek(SO_FILE *stream, long offset, int whence){
    if (stream->prev == READprev){
        so_fflush(stream);
    }
    if (stream->prev == WRITEprev){
        so_fflush(stream);
    }
    stream->cursor = lseek(stream->so_fd, offset, whence);
    if(stream->cursor < 0)
    {
        stream->isERR = 555;
        return SO_EOF;
    }else if (stream->cursor > stream->so_sizeFile){
        stream->isERR = 555;
        return SO_EOF;
    }
        return 0;
    return SO_EOF;
}