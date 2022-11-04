#include "so_stdio.h"

int so_fileno(SO_FILE *stream){
    return stream->so_fd;
}