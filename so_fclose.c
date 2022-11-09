#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

int so_fclose(SO_FILE *stream)
{
    if (stream != NULL)
    {
        if (stream->prev == WRITEprev){
            //printf("fflush start\n");
            so_fflush(stream);
            //printf("fflush end\n");
        }
        //printf("fd: %d\n", stream->so_fd);
        int k = close(stream->so_fd);
        if (k == -1){
            //stream->isERR = 555;
            free(stream);
            return  SO_EOF;
        }
        //stream->isERR = 555;
        free(stream);
        return 0;
    }else {return SO_EOF; }//stream->isERR = 555;}
}