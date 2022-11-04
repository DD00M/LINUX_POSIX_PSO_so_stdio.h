#include "so_stdio.h"
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

#define __linux__

int main(){
    int ch;

    SO_FILE*p = so_popen("pwd", "r");
    printf("%d\n", p->so_fd);
    printf("%d\n", p->so_fd);
    if( p == NULL)
    {
        puts("Unable to open process");
        return(1);
    }
    printf("1\n");
    char *buffer = (char*)malloc(20*sizeof(char));

    printf("2\n");
    so_fread(buffer, 20, 1, p);
    printf("%s\n", buffer);

    //pclose(p);
    printf("%d", p->so_fd);
    return(0);


    return 0;
}
