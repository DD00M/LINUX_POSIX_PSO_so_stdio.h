#include "so_stdio.h"
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

int main(){
    int ch;

    SO_FILE*p = so_popen("pwd", "r");
    if (p == NULL){
        printf("da");
    }
    //pclose(p);
    printf("%d", p->so_fd);
    return(0);


    return 0;
}
