#include "so_stdio.h"

int so_fseek(SO_FILE *stream, long offset, int whence){
    if (stream->prev == READprev){
        stream->buffer_index = 0;
    }
    if (stream->prev == WRITEprev){
        so_fflush(stream);
    }
    if (whence == SEEK_SET){
        stream->cursor = offset;
        if (stream->cursor < 0 || stream->cursor > stream->so_sizeFile){
            perror("out of bounds\n");
            return SO_EOF;
        }
        return 0;
    }else if (whence == SEEK_CUR){
        stream->cursor = stream->cursor + offset;
        if (stream->cursor < 0 || stream->cursor > stream->so_sizeFile){
            perror("out of bounds\n");
            return SO_EOF;
        }
        return 0;
    }else if (whence == SEEK_END){ 
        stream->cursor = stream->so_sizeFile - offset;
        if (stream->cursor < 0 || stream->cursor > stream->so_sizeFile){
            perror("out of bounds\n");
            return SO_EOF;
        }
        return 0;
    }
    return SO_EOF;
}