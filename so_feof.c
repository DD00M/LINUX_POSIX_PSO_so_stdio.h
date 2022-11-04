#include "so_stdio.h"

int so_feof(SO_FILE *stream)
{
    if (stream->buffer_index == stream->so_sizeFile){
        return SO_EOF;
    }
}