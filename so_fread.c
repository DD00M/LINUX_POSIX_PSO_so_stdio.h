#include <unistd.h>
#include <fcntl.h>  /* open, O_RDONLY */
#include <errno.h>  /* errno, perror */
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "so_stdio.h"

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream){
    if (nmemb <= 0 || size <= 0){
        perror("invalid nmemb or size\n");
        return 0;
    }else{
        if (stream == NULL){
            perror("bad stream\n");
            return 0;
        }else{
            int t = 0;
            int i = 0;
            char *aux = (char*)ptr;
            for (int j = 0; j < nmemb; j++){
                for (i = t; i < size; i++){
                    int k = so_fgetc(stream);
                    aux[i] = (char)k;
                    printf("%c\n", aux[i]);
                }
                t = i;
            }
            printf("\n%s", aux);
            printf("\n%s", ptr);
            stream->read_flag = 1;
            stream->prev = READprev;
            return size*nmemb;
        }
    }
}