.PHONY: clean
so_fopen.o: so_fopen.c
	gcc -fPIC -c so_fopen.c -o so_fopen.o
so_fclose.o: so_fclose.c
	gcc -fPIC -c so_fclose.c -o so_fclose.o
so_fputc.o: so_fputc.c
	gcc -fPIC -c so_fputc.c -o so_fputc.o
so_fgetc.o: so_fgetc.c
	gcc -fPIC -c so_fgetc.c -o so_fgetc.o
so_fflush.o: so_fflush.c
	gcc -fPIC -c so_fflush.c -o so_fflush.o
so_ferror.o: so_ferror.c
	gcc -fPIC -c so_ferror.c -o so_ferror.o
so_feof.o: so_feof.c
	gcc -fPIC -c so_feof.c -o so_feof.o
so_fileno.o: so_fileno.c
	gcc -fPIC -c so_fileno.c -o so_fileno.o
so_ftell.o: so_ftell.c
	gcc -fPIC -c so_ftell.c -o so_ftell.o
so_fread.o: so_fread.c
	gcc -fPIC -c so_fread.c -o so_fread.o
so_fwrite.o: so_fwrite.c
	gcc -fPIC -c so_fwrite.c -o so_fwrite.o
so_fseek.o: so_fseek.c
	gcc -fPIC -c so_fseek.c -o so_fseek.o
so_popen.o: so_popen.c
	gcc -fPIC -c so_popen.c -o so_popen.o
libso_stdio.so: so_fopen.o so_fclose.o so_fputc.o so_fgetc.o so_fileno.o so_fflush.o so_feof.o so_ferror.o so_ftell.o so_fread.o so_fwrite.o so_popen.o so_fseek.o
	gcc -shared so_fopen.o so_fclose.o so_fputc.o so_fgetc.o so_fileno.o so_fflush.o so_feof.o so_ferror.o so_ftell.o so_fread.o so_fwrite.o so_popen.o so_fseek.o -o libso_stdio.so
main: main.c libso_stdio.so
	gcc main.c -o main -l so_stdio -L .
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
clean: 
	rm libso_stdio.so so_fopen.o so_fclose.o so_feof.o so_ferror.o so_fflush.o so_fgetc.o so_fileno.o so_fputc.o so_ftell.o so_fwrite.o so_popen.o so_fseek.o main
