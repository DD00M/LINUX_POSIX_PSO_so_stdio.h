#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

#include <stdio.h>

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
    if (strcmp(stream->mode, "r") == 0){
        return SO_EOF;
    }
    if (nmemb <= 0 || size <= 0)
    {
        stream->isERR = 555;
        return 0; 
    }
    else
    {
        if (stream->read_flag == 1){
            stream->buffer_index = 0;
            stream->read_flag = 0;
        }
        if (stream == NULL)
        {
            stream->isERR = 555;
            return 0;
        }
        else
        {
            char *aux = (char *)ptr;

            for (int i = 0; i < size * nmemb; i++){
                so_fputc((int)aux[i], stream);
            }

            stream->prev = WRITEprev;
            stream->isERR = 888;
            return size * nmemb;
        }
    }
    stream->isERR = 555;
    return SO_EOF;
}