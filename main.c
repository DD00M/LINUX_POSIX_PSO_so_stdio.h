#include "so_stdio.h"
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>

int main(){
    int ch;

    SO_FILE*p = so_popen("pwd", "r");
    //SO_FILE*file = so_fopesn("out.txt", "r");

    //p//rintf("%d", (int)so_fwrite("ana are mere", 13, 1, file));

    printf("%d\n", p->so_fd);
    if( p == NULL)
    {
        printf("Unable to open process\n");
    }
    char *buffer = (char*)malloc(20*sizeof(char));
    //so_pclose(p);
    so_fread(buffer, 18, 1, p);
    printf("%s\n", buffer);

    //pclose(p);
    //printf("%d", p->so_fd);

    return 0;
}
