#include "so_stdio.h"

int so_ferror(SO_FILE *stream)
{
    if (stream->isERR == 222)
    {
        return 222;
    }
    else if (stream->isERR == 333)
    {
        return 333;
    }
    else if (stream->isERR == 444)
    {
        return 444;
    }
    return SO_EOF;
}