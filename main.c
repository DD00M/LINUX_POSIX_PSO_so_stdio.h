#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "so_stdio.h"
#include "test_util.h"

#include "hooks.h"

char buf[] = "Hello, World!\n";
int buf_len = sizeof(buf) - 1;

int main(int argc, char *argv[])
{
	SO_FILE *f;
	int c;
	int ret;
	char *test_work_dir;
	char fpath[256];
	int i;


	/* --- BEGIN TEST --- */
	f = so_fopen("./binar", "r");

	int k = so_fileno(f);

	for (i = 0; i < buf_len; i++) {
		c = so_fgetc(f);
    printf("%c\n", c);
    printf("%d %d %d\n", i, buf_len, strlen(buf));
		ret = so_feof(f);
		printf("%d\n", ret);
	}

	return 0;
}