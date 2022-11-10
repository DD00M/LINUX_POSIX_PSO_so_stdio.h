#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream){
    if (strcmp(stream->mode, "w") == 0){
        return SO_EOF;
    }  
    if (stream->is_file != 1){
        return SO_EOF;
    }
    //printf("a intrat si aici\n");  
    
    if (strcmp(stream->mode, "a") == 0){
        stream->cursor = lseek(stream->so_fd, 0, SEEK_END);
    }
    
    if (strcmp(stream->mode, "a+") == 0){
        lseek(stream->so_fd, 0, SEEK_END);
    }

    if (nmemb <= 0 || size <= 0){
        return SO_EOF;
    }
    if (stream == NULL){
        return SO_EOF;
    }
        //printf("a intrat si aici1\n");  
    char *aux = ptr;
        //printf("nmemb, size: %d %d\n", nmemb, size);

    int c = 0;
    int s = 0;
    int i = 0;
    int p = 0;
    int lala = 0;
    if (size*nmemb >= stream->so_sizeFile){
       lala = stream->so_sizeFile;
    }else {lala = size * nmemb; }
    //printf("SIZEFILE: %d\n", stream->so_sizeFile);
    while (i < lala)
    {
        //for (int j = 0; j < size; j++){
            c = so_fgetc(stream);
            if (c == SO_EOF && stream->flagERR == -1){
                return i / size;
            }
            aux[p] = (char)c;
            p++;
        //}
        i++;
    }


            //printf("a intrat si aici4\n");  
    stream->read_flag = 1;
    stream->prev = READprev;
            //printf("a intrat si aici3\n");  
            //printf("XXXXXXX: %d\n", size*nmemb);
    return i / size;
}