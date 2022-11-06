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
        perror("invalid nmemb or size\n");
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
            perror("bad stream\n");
            return 0;
        }
        else
        {
            char *aux = (char *)ptr;
            int t = 0;
            int i = 0;
            for (int j = 0; j < nmemb; j++)
            {
                for (int i = t; i < size; i++)
                {
                    if(stream->buffer_index == BUFSIZE){
                        i--;
                    }
                    so_fputc((int)aux[i], stream);
                }
                t = i;
            }
            stream->prev = WRITEprev;
            return size * nmemb;
        }
    }
}