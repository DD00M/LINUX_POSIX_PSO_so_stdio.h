#include "so_stdio.h"

long so_ftell(SO_FILE *stream){
    return stream->cursor;
}