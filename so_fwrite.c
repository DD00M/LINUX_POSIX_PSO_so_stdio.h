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
        stream->isERR = 999;
        return 0;
    }
    if (nmemb <= 0 || size <= 0)
    {
        stream->isERR = 999;
        return 0; 
    }
    if (stream->read_flag == 1){
        stream->buffer_index = 0;
        stream->read_flag = 0;
    }
    if (stream == NULL)
    {
        stream->isERR = 999;
        return 0;
    }
    char *aux = (char *)ptr;
    int c = 0;
    int s = 0;
    int i = 0;
    int p = 0;
    int lala = 0;

    while (i < nmemb)
    {
        s = size;
        do
        {
            so_fputc((int)aux[p], stream);
            p++;
        }while(--s);
        i++;
    }

    stream->prev = WRITEprev;
    stream->isERR = 888;
    return i;
}