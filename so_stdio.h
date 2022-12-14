/*
 * Operating System Executable Loader header
 *
 * 2019, Operating Systems
 */

#ifndef SO_STDIO_H
#define SO_STDIO_H

#if defined(__linux__)
#define FUNC_DECL_PREFIX
#elif defined(_WIN32)
#include <Windows.h>

#ifdef DLL_EXPORTS
#define FUNC_DECL_PREFIX __declspec(dllexport)
#else
#define FUNC_DECL_PREFIX __declspec(dllimport)
#endif

#else
#error "Unknown platform"
#endif

#include <stdlib.h>

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

#define READprev 0
#define WRITEprev 1

#define SO_EOF (-1)

struct _so_file;

#define BUFSIZE 4096

typedef struct _so_file{
    int so_start_offset;
    int so_fd;
    int so_sizeFile;
    char mode[2];
    char buffer[BUFSIZE];
    char currentBufSize;
    int buffer_index;
    int off_read;
    int off_written;
    int isERR;

    int firstIndex;
    int lastIndex;
    int bufsizeIndex;

    int read_flag;

    long cursor;

    int prev;

    int pid;
    int ppid;

    int is_p;
    int childFlag;

    int is_file;
    int flagERR;

    int eof;
} SO_FILE;

FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode);
FUNC_DECL_PREFIX int so_fclose(SO_FILE *stream);

#if defined(__linux__)
FUNC_DECL_PREFIX int so_fileno(SO_FILE *stream);
#elif defined(_WIN32)
FUNC_DECL_PREFIX HANDLE so_fileno(SO_FILE *stream);
#else
#error "Unknown platform"
#endif


FUNC_DECL_PREFIX int so_fflush(SO_FILE *stream);

FUNC_DECL_PREFIX int so_fseek(SO_FILE *stream, long offset, int whence);
FUNC_DECL_PREFIX long so_ftell(SO_FILE *stream);

FUNC_DECL_PREFIX
size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream);

FUNC_DECL_PREFIX
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream);

FUNC_DECL_PREFIX int so_fgetc(SO_FILE *stream);
FUNC_DECL_PREFIX int so_fputc(int c, SO_FILE *stream);

FUNC_DECL_PREFIX int so_feof(SO_FILE *stream);
FUNC_DECL_PREFIX int so_ferror(SO_FILE *stream);

FUNC_DECL_PREFIX SO_FILE *so_popen(const char *command, const char *type);
FUNC_DECL_PREFIX int so_pclose(SO_FILE *stream);

#endif /* SO_STDIO_H */
