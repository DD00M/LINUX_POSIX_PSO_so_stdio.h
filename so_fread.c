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
    //printf("a intrat si aici\n");  
    if (nmemb <= 0 || size <= 0){
        return 0;
    }else{
        if (stream == NULL){
            return 0;
        }else{
            //printf("a intrat si aici1\n");  
            char *aux = (char*)ptr;
            //printf("nmemb, size: %d %d\n", nmemb, size);

            int z = 0;

            if (size * nmemb > stream->so_sizeFile){
                z = stream->so_sizeFile;
            }else z = size * nmemb;

            for (int i = 0; i < size * nmemb; i++){ 
                int k = so_fgetc(stream);
                aux[i] = (char)k;
            }
            //printf("a intrat si aici4\n");  
            stream->read_flag = 1;
            stream->prev = READprev;
            //printf("a intrat si aici3\n");  
            //printf("XXXXXXX: %d\n", size*nmemb);
            return z;
        }
    }
}