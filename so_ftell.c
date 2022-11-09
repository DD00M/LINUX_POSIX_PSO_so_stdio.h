#include "so_stdio.h"

long so_ftell(SO_FILE *stream)
{
    if (stream != NULL)
    {
        stream->isERR = 888;
        return stream->cursor;
    }
    stream->isERR = 555;
    return -1;
}