#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define PORT 8080
#define BUFSIZE 1024
#define num_files 1024
#define num_lines 256
#define nr_of_words 10

#define isLIST 1
#define isGET 2
#define isPUT 3
#define isDELETE 4
#define isUPDATE 5
#define isSEARCH 6

#define logFile "./log.txt"

int number_of_files = 0;
int number_of_readFiles = 0;

int control_operationValue = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_signal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_signal = PTHREAD_COND_INITIALIZER;
int condition_spin = 0;
int signal_condition = 0;
int sigsegv_condition = 0;
int flag = 0;

time_t current_time;

char *files[num_files];
char *readFiles[num_files];

char buffer[BUFSIZE] = {0};

//return value for join
int val = -999;

//search word
char *toSearchWord;

// update vars
char *filename;
off_t offset_update;
int sizeFile_update;

//thread index:
pthread_t th_id_spin;
pthread_t tid;

int server_fd, new_socket;

static int mask[] = {SIGTERM, SIGINT};

int counter = 0;

struct mostRecentWords
{
    char name[BUFSIZE];
    char words[nr_of_words][BUFSIZE];
    int frequency[nr_of_words];
};

typedef struct mostRecentWords mostRecentWords_T;

mostRecentWords_T *my_fileStruct;

bool take_only_ExtFile(char *strr)
{
    if (strstr(strr, ".") != NULL || strstr(strr, ".exe") != NULL || strstr(strr, ".o") != NULL || strstr(strr, ".i") != NULL)
    {
        return true;
    }
    return false;
}

void bubble_sort_BYSTRINGS(char *strings[], int *ints, size_t len)
{
    int temp_ints;
    char *temp_strings;
    for (size_t pass = 0; pass < len - 1; ++pass)
    {
        for (size_t n = 0; n < len - 1; ++n)
        {
            if (strcmp(strings[n], strings[n + 1]) > 0 && strings[n + 1] != 0x00)
            {
                temp_ints = ints[n];
                ints[n] = ints[n + 1];
                ints[n + 1] = temp_ints;

                temp_strings = strdup(strings[n]);
                strcpy(strings[n], strings[n + 1]);
                strcpy(strings[n + 1], temp_strings);
            }
        }
    }
}

void bubble_sort_BYINTS(char *strings[], int *ints, size_t len)
{
    int temp_ints;
    char *temp_strings;
    for (size_t pass = 0; pass < len - 1; ++pass)
    {
        for (size_t n = 0; n < len - 1; ++n)
        {
            if (ints[n] < ints[n + 1] && strings[n + 1] != 0x00)
            {
                temp_ints = ints[n];
                ints[n] = ints[n + 1];
                ints[n + 1] = temp_ints;

                temp_strings = strdup(strings[n]);
                strcpy(strings[n], strings[n + 1]);
                strcpy(strings[n + 1], temp_strings);
            }
        }
    }
}

void removeDuplicates(char **arr, int x[], int n)
{
    int j = 0;

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            // If any duplicate found
            if (arr[i] != 0x00 && arr[j] != 0x00)
            {
                if (strcmp(arr[i], arr[j]) == 0)
                {
                    // Delete the current duplicate element
                    for (int k = j + 1; k < n; k++)
                    {
                        if (arr[k - 1] != 0x00 && arr[k] != 0x00)
                        {
                            strcpy(arr[k - 1], arr[k]);
                            x[k - 1] = x[k];
                        }
                    }    // Decrement size of array(n) after removing duplicate element
                    n--;
                    j--;
                }
            }
        }
    }
}

void populate_struct_with_MostRecentWords(mostRecentWords_T *myStr)
{
    const int word_count_max = 10000;
    char c;
    char word[BUFSIZE];
    char **p;
    int auxFreq[BUFSIZE * 5];
    p = (char **)malloc(5 * BUFSIZE * sizeof(char *));
    for (int i = 0; i < number_of_readFiles; i++)
    {
        for (int j = 0; j < 5 * BUFSIZE; j++)
        {
            p[j] = NULL;
        }
        FILE *fd = fopen(readFiles[i], "r");
        if (fd == NULL)
        {
            printf("error opening %s file\n", files[i]);
        }
        int k = 0;
        int word_array_index = 0;
        while ((c = fgetc(fd)) != EOF)
        {
            if (isalpha(c))
            {
                word[k] = c;
                k++;
            }
            else
            {
                if (k != 0)
                {
                    word[k++] = '\0';
                    p[word_array_index] = strdup(word);
                    word_array_index++;
                    k = 0;
                    memset(word, 0, sizeof(word));
                }
            }
        }
        int currentFrq = 0;
        k = 0;
        for (int z = 0; z < word_array_index; z++)
        {
            for (int x = 0; x < word_array_index; x++)
            {
                if (strcmp(p[z], p[x]) == 0)
                {
                    currentFrq++;
                }
            }
            currentFrq -= 1;
            auxFreq[k] = currentFrq + 1;
            currentFrq = 0;
            k++;
        }
        bubble_sort_BYINTS(p, auxFreq, BUFSIZE);
        removeDuplicates(p, auxFreq, BUFSIZE);
        int size = BUFSIZE;
        int c = 0;
        strcpy(myStr[i].name, readFiles[i]);
        for (int b = 0; b < nr_of_words; b++){
            if (strcmp(p[b], p[b + 1]) == 0){
                myStr[i].frequency[b] = -1;
            strcpy(myStr[i].words[b], "-");
            }else{
            myStr[i].frequency[b] = auxFreq[b];
            strcpy(myStr[i].words[b], p[b]);
            }
        }
    }
    for (int i = 0; i < 5*BUFSIZE; i++){
        free(p[i]);
    }
    free(p);
}

void write_in_log(char *tip_operatie)
{
    pthread_mutex_lock(&mutex);
    time(&current_time);
    char *logString = (char *)malloc(BUFSIZE * sizeof(char));

    int logFd = open(logFile, O_RDWR | O_APPEND);
    if (logFd < 0)
    {
        printf("error at opening logFile\n");
        return;
    }
    int sizerr = 0;
    memset(logString, 0, BUFSIZE);
    if (strcmp(tip_operatie, "LIST") == 0)
    {
        // char result_integer[50];
        // sprintf(result_integer, "%ld", ctime(&current_time));
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        sizerr += strlen(ctime_no_newline) + 8;
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "\n");
    }
    else if (strcmp(tip_operatie, "GET") == 0)
    {
        // char result_integer[50];
        // sprintf(result_integer, "%ld", ctime(&current_time));
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "<->");
        strcat(logString, filename);
        strcat(logString, "\n");
        sizerr += strlen(ctime_no_newline) + 2 * 3 + 3 + strlen(filename) + 1;
    }
    else if (strcmp(tip_operatie, "PUT") == 0)
    {
        // char result_integer[50];
        // sprintf(result_integer, "%ld", ctime(&current_time));
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "<->");
        strcat(logString, filename);
        strcat(logString, "\n");
        sizerr += strlen(ctime_no_newline) + 2 * 3 + 3 + strlen(filename) + 1;
        printf("lallslasdlasdlasdlas: %d", sizerr);
    }
    else if (strcmp(tip_operatie, "REMOVE") == 0)
    {
        // char result_integer[50];
        // sprintf(result_integer, "%ld", time(&current_time));
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "<->");
        strcat(logString, filename);
        strcat(logString, "\n");
        sizerr += strlen(ctime_no_newline) + 2 * 3 + 6 + strlen(filename) + 1;
    }
    else if (strcmp(tip_operatie, "UPDATE") == 0)
    {
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "<->");
        strcat(logString, filename);
        strcat(logString, "\n");
        sizerr += strlen(ctime_no_newline) + 2 * 3 + 6 + strlen(filename) + 1;
    }
    else if (strcmp(tip_operatie, "SEARCH") == 0)
    {
        printf("inside\n");
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "<->");
        strcat(logString, toSearchWord);
        strcat(logString, "\n");
        sizerr += strlen(ctime_no_newline) + 2 * 3 + 6 + strlen(toSearchWord) + 1;
        printf("what happened\n");
    }
    else if (strcmp(tip_operatie, "EXIT") == 0)
    {
        char *ctime_no_newline;
        char *rest = strdup(ctime(&current_time));
        ctime_no_newline = strtok_r(ctime(&current_time), "\n", &rest);
        sizerr += strlen(ctime_no_newline) + 8;
        strcpy(logString, ctime_no_newline);
        strcat(logString, "<->");
        strcat(logString, tip_operatie);
        strcat(logString, "\n");
    }
    printf("log: %s\n", logString);
    write(logFd, logString, sizerr);
    pthread_mutex_unlock(&mutex);
    printf("endeddd\n");
}

int get_files()
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    int i = 0;
    int j = 0;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            //printf("KKK: %s\n", dir->d_name);
            if (dir->d_type == DT_REG)
            {
                //printf("xxx: %s\n", dir->d_name);
                files[i] = strdup(dir->d_name);
                if (take_only_ExtFile(files[i]) == true)
                {
                    readFiles[j] = strdup(files[i]);
                    j++;
                }
                i++;
            }
        }
        closedir(d);
    }
    number_of_files = i;
    //for (int i = 0; i < number_of_files; i++){
    //    printf("%s----\n", files[i]);
    //}
    number_of_readFiles = j;
    return i;
}

/*
void clear_GET_list()
{
    for (int i = 0; i < num_files; i++)
    {
        free(file_lines[i]);
        file_lines[i] = NULL;
    }
}
*/

/*
void read_from_file_and_populate(const char *file_name)
{
    clear_GET_list();
    FILE *filePointer;
    int bufferLength = 255;
    char bufferx[bufferLength]; /* not ISO 90 compatible

    filePointer = fopen(file_name, "r");
    int k = 0;
    while (fgets(bufferx, bufferLength, filePointer))
    {
        file_lines[k] = strdup(bufferx);
        k++;
    }
    number_of_lines_perFile = k + 1;
    fclose(filePointer);
}

*/

char *get_result(char *my_str, int statusValue)
{
    char *myStr = (char *)malloc(BUFSIZE * sizeof(char));
    char *status = NULL;
    if (statusValue == 0)
    {
        status = strdup("Success");
    }
    else
        status = strdup("Failed");

    strcpy(myStr, status);
    strcat(myStr, "!");

    char result_integer[50];
    sprintf(result_integer, "%ld", sizeof(myStr));

    strcat(myStr, result_integer);
    strcat(myStr, "!");
    strcat(myStr, my_str);

    return myStr;
}

void free_spinning_func(mostRecentWords_T* strct){
    for (int i = 0; i < number_of_files; i++){
        free(files[i]);
    }
    //free(files);
    for (int i = 0; i < number_of_readFiles; i++)
    {
        free(readFiles[i]);
    }
    //free(readFiles);
    printf("Errthang freed\n");
    //for (int i = 0; i < number_of_readFiles; i++){
    free(strct);
    //}

}

static void ask_handler(int signo)
{
    char bufferx[128];

    printf("Got %d  - Stop program? [Y/n] ", signo);
    fflush(stdout);
    fgets(bufferx, 128, stdin);
    bufferx[strlen(bufferx) - 1] = '\0';
    if (bufferx[0] == 'y' || bufferx[0] == 'Y')
    {
        pthread_mutex_lock(&mutex_signal);
        printf("1\n");
        if (counter == 0){
            counter = 0;
        }else
            counter--;
        signal_condition = 1;
        pthread_mutex_unlock(&mutex_signal);

        //printf("after shutdown\n");
        pthread_join(tid, NULL);
       // printf("2\n");
        free_spinning_func(my_fileStruct);
        //printf("3\n");
        pthread_cancel(tid);
        //printf("4\n");

        shutdown(new_socket, SHUT_RDWR);
        shutdown(server_fd, SHUT_RDWR);

        printf("Client %d disconnected\n", counter);

        exit(EXIT_SUCCESS);
    }else{
        printf("wrong command\n");
    }
}

static void set_signals(void)
{
    struct sigaction sa;
    int rc;
    sigset_t m_mask;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&m_mask);

    /* TODO - set handler in struct sigaction sa */

    sa.sa_handler = ask_handler;
    sa.sa_mask = m_mask;
    sa.sa_flags = SA_SIGINFO;
    /* TODO - handle SIGINT, SIGQUIT and SIGUSR1 signals */

    for (int i = 0; i < sizeof(mask) / sizeof(int); i++)
    {
        rc = sigaction(mask[i], &sa, NULL);
        if (rc == -1)
        {
            printf("sigact err\n");
        }
    }
}

void signal_handler2(){
    shutdown(new_socket, SHUT_RDWR);
    //shutdown(server_fd, SHUT_RDWR);
    printf("Server closed comunication for socket\n");
    exit(EXIT_SUCCESS);
}

void set_signals2(){
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_handler = signal_handler2;

    sigaction(SIGSEGV, &sa, NULL);
}

char **get_string_splitted(char *input, const char *chr)
{
    char **toRet = (char **)malloc(num_files * sizeof(char *));
    for (int i = 0; i < num_files; i++)
    {
        toRet[i] = NULL;
    }
    int k = 0;
    char *rest = strdup(input);
    char *token = strtok_r(input, chr, &rest);

    while (token != NULL)
    {
        toRet[k] = strdup(token);
        k++;
        token = strtok_r(NULL, chr, &rest);
    }
    return toRet;
}

void *pthread_GET(int new_socket)
{
    pthread_mutex_lock(&mutex);

    void *result;
    // printf("before split\n");
    char *myBuf = (char *)malloc(sizeof(buffer) * sizeof(char));
    strcpy(myBuf, buffer);
    char **split = get_string_splitted(myBuf, "!");
    // printf("after split\n");
    // printf("%s-%s-%s\n", split[0], split[1], split[2]);
    filename = NULL;
    filename = strdup(split[2]);
    control_operationValue = isGET;
    char *auxBuffer = (char *)malloc(BUFSIZE * sizeof(char));
    memset(auxBuffer, 0, sizeof(auxBuffer));
    if (access(filename, F_OK) == 0)
    {
        int in_fd = open(filename, O_RDONLY);

        struct stat st;
        stat(filename, &st);
        int sizeFile = st.st_size;
        char result_integer[50];
        sprintf(result_integer, "%d", sizeFile);
        strcpy(auxBuffer, "Success");
        strcat(auxBuffer, "!");
        strcat(auxBuffer, result_integer);
        // printf("buffer primul send: %s\n", auxBuffer);
        send(new_socket, auxBuffer, BUFSIZE, 0);
        ;
        sendfile(new_socket, in_fd, 0, sizeFile + 1);
        // send(new_socket, myStr, strlen(myStr), 0);

        close(in_fd);
        pthread_mutex_unlock(&mutex);
        write_in_log("GET");
    }
    else
    {
        char result_integer[50];
        sprintf(result_integer, "%d", 0);
        strcpy(auxBuffer, "Failed");
        strcat(auxBuffer, "!");
        strcat(auxBuffer, result_integer);
        send(new_socket, auxBuffer, sizeof(auxBuffer), 0);
        printf("File not found\n");
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
}

void *pthread_LIST(int new_socket)
{
    pthread_mutex_lock(&mutex);

    int nr_of_files = get_files();
    int total_length = 0;

    printf("Numarul de fisiere este: %d\n", nr_of_files);

    // for (int i = 0; i < nr_of_files; i++)
    // {
    //    printf("%s\n", files[i]);
    //}

    char *return_list = (char *)malloc(BUFSIZE * sizeof(char));
    strcpy(return_list, files[0]);
    strcat(return_list, "\\0");
    for (int i = 1; i < nr_of_files; i++)
    {
        strcat(return_list, files[i]);
        strcat(return_list, "\\0");
        if (i == nr_of_files - 1){
            strcat(return_list, files[i]);
        }
    }
    //printf("RETURN LIST: %s\n", return_list);
    control_operationValue = isLIST;
    pthread_mutex_unlock(&mutex);
    //printf("before log\n");
    write_in_log("LIST");
    //printf("after log\n");
    char *myStr = get_result(return_list, 0);
    char result_integerx[10];
    sprintf(result_integerx, "%d", nr_of_files);
    strcat(myStr, "!");
    strcat(myStr, result_integerx);
    send(new_socket, myStr, strlen(myStr), 0);
    memset(buffer, 0, sizeof(buffer));
}

void *pthread_PUT(int new_socket, int sizeInputFile)
{
    pthread_mutex_lock(&mutex);
    // printf("1\n");
    char reader[sizeInputFile + 1];
    recv(new_socket, reader, sizeInputFile + 1, 0);
    // printf("2\n");
    int out_file = open(filename, O_WRONLY | O_CREAT, 0777);
    int rc = write(out_file, reader, sizeInputFile);
    char status[32] = {0};
    if (rc > 0)
    {
        strcpy(status, "Success");
        send(new_socket, status, sizeof(status), 0);
    }
    else
    {
        strcpy(status, "Failed");
        send(new_socket, status, sizeof(status), 0);
    }

    flag = 1;
    pthread_cond_signal(&cond);

    close(out_file);
    pthread_mutex_unlock(&mutex);
    write_in_log("PUT");
    pthread_mutex_lock(&mutex);
    condition_spin = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

void *pthread_DELETE(int new_socket)
{
    // printf("0\n");
    pthread_mutex_lock(&mutex);
    char status[32] = {0};
    if (access(filename, F_OK) == 0)
    {
        //printf("before remove\n");
        int x;
        x = remove(filename);
        //printf("after remove\n");
        if (x == 0)
        {
            //printf("succes\n");
            strcpy(status, "Success");
            send(new_socket, status, sizeof(status), 0);
            printf("a dat send\n");
        }
        else
        {
            strcpy(status, "Failed");
            send(new_socket, status, sizeof(status), 0);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }
    else
    {
        strcpy(status, "Failed");
        send(new_socket, status, sizeof(status), 0);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    flag = 1;
    pthread_cond_signal(&cond);
    // printf("a terminat\n");
    pthread_mutex_unlock(&mutex);
    write_in_log("PUT");
    pthread_mutex_lock(&mutex);
    condition_spin = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *pthread_UPDATE(int socket)
{
    pthread_mutex_lock(&mutex);
    char status[32] = {0};
    char *string_received = (char *)malloc((sizeFile_update + 1) * sizeof(char));
    memset(string_received, 0, sizeof(string_received));

    recv(socket, string_received, sizeFile_update + 1, 0);

    int to_updateFD = open(filename, O_WRONLY);
    if (to_updateFD < 0)
    {
        printf("file to update failed to open\n");
        strcpy(status, "Failed");
        send(socket, status, 32, 0);
    }

    int rc = pwrite(to_updateFD, string_received, sizeFile_update, offset_update);
    if (rc < 0)
    {
        printf("write failed\n");
        strcpy(status, "Failed");
        send(socket, status, 32, 0);
    }
    else
    {
        strcpy(status, "Success");
        send(socket, status, 32, 0);
    }

    // printf("fisier to change: \n%s", string_received);

    pthread_mutex_unlock(&mutex);
    write_in_log("UPDATE");
    pthread_mutex_lock(&mutex);
    condition_spin = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *pthread_SEARCH(int socket){
    pthread_mutex_lock(&mutex);
    bool isFound = false;
    bool didStrcpy = false;
    char status[64] = {0};
    char toSendString[BUFSIZE];
    int howManyFiles = 0;
    int check = 0;
    //memset(check, 0, number_of_readFiles);
    for (int i = 0; i < number_of_readFiles; i++){
        for (int j = 0; j < nr_of_words; j++){
            //printf("-%s- -%s-\n", my_fileStruct[i].words[j], toSearchWord);
            if (strcmp(my_fileStruct[i].words[j], toSearchWord) == 0){
                check = 1;
                isFound = true;
                if (didStrcpy == false){
                    strcpy(toSendString, my_fileStruct[i].name);
                    strcat(toSendString, "\\0");
                    didStrcpy = true;
                }else{
                    strcat(toSendString, my_fileStruct[i].name);
                    strcat(toSendString, "\\0");
                }
                howManyFiles++;
            }
        }
    }

    if (check == 0){
        strcpy(status, "Failed");
        send(socket, status, 64, 0);
    }else{
        //printf("a intrat pe succes\n");
        strcpy(status, "Success");
        strcat(status, "!");
        char result_integer[10];
        sprintf(result_integer, "%d", howManyFiles);
        strcat(status, result_integer);
        //printf("status: %s\n", status);
        send(socket, status, 64, 0);
        //printf("%s\n", toSendString);
        send(socket, toSendString, 1024, 0);
    }
    memset(toSearchWord, 0, sizeof(toSearchWord));
    pthread_mutex_unlock(&mutex);
    //printf("inainte de log\n");
    write_in_log("SEARCH");
}

void *thread_spinning_func()
{
    while (1) 
    {
        pthread_mutex_lock(&mutex);
        while (!condition_spin) {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("Thread woke up!\n");
        for (int i = 0; i < number_of_files; i++)
        {
            files[i] = NULL;
        }

        number_of_files = get_files();
        //printf("got files: %d %d\n", number_of_files, number_of_readFiles);
        populate_struct_with_MostRecentWords(my_fileStruct);

        pthread_mutex_unlock(&mutex);
        condition_spin = 0;
    }
}

void *pthread_exec_func(void *arg)
{
    while (1)
    {
        int new_socket = *((int *)arg);
        memset(buffer, 0, sizeof(buffer));
        //printf("ZZZZZZZZZZZZZZZZZZZZZZZZZZ: %d\n", signal_condition);
        if (signal_condition == 1) {
            //printf("a iesit\n");
            void *status;
            pthread_exit(status);
        }
        //printf("SALUT\n");
        int valread = recv(new_socket, buffer, BUFSIZE, 0);
        printf("Client %d sent: %s\n", counter, buffer);
        char *myBuf = (char *)malloc(sizeof(buffer) * sizeof(char));
        // printf("x\n");
        strcpy(myBuf, buffer);
        char **split = get_string_splitted(myBuf, "!");
        if (strcmp(split[0], "LIST") == 0 || strcmp(split[0], "list") == 0)
        {
            printf("a intrat pe list\n");
            // return
            pthread_LIST(new_socket);
        }
        else if (strcmp(split[0], "GET") == 0 || strcmp(split[0], "get") == 0)
        {
            pthread_mutex_lock(&mutex);
            filename = NULL;
            filename = strdup(split[2]);
            // printf("a intrat pe get\n");
            pthread_mutex_unlock(&mutex);
            // return
            pthread_GET(new_socket);
        }
        else if (strcmp(split[0], "PUT") == 0 || strcmp(split[0], "put") == 0)
        {
            // printf("0\n");
            int sizeInputFilePUT = atoi(split[3]);
            // printf("size: %d\n", sizeInputFilePUT);

            pthread_mutex_lock(&mutex);
            filename = NULL;
            filename = strdup(split[2]);
            pthread_mutex_unlock(&mutex);

            // return
            pthread_PUT(new_socket, sizeInputFilePUT);
        }
        else if (strcmp(split[0], "DELETE") == 0 || strcmp(split[0], "delete") == 0)
        {
            // printf("0x\n");
            pthread_mutex_lock(&mutex);
            filename = NULL;
            // printf("1x\n");
            filename = strdup(split[2]);
            // printf("2x\n");
            pthread_mutex_unlock(&mutex);
            // printf("a intrat in delete\n");
            // return
            pthread_DELETE(new_socket);
        }
        else if (strcmp(split[0], "UPDATE") == 0 || strcmp(split[0], "update") == 0)
        {
            // printf("0x\n");
            pthread_mutex_lock(&mutex);
            filename = NULL;
            // printf("1x\n");
            filename = strdup(split[2]);
            // printf("2x\n");
            offset_update = atoi(split[3]);
            sizeFile_update = atoi(split[4]);
            pthread_mutex_unlock(&mutex);
            // printf("a intrat in delete\n");
            // return
            pthread_UPDATE(new_socket);
        }
        else if (strcmp(split[0], "SEARCH") == 0 || strcmp(split[0], "search") == 0)
        {
            //printf("0x\n");
            pthread_mutex_lock(&mutex);
            toSearchWord = NULL;
            //printf("1x\n");
            toSearchWord = strdup(split[2]);
            //printf("2x\n");
            //printf("saluttt: %s\n", toSearchWord);
            pthread_mutex_unlock(&mutex);
            //printf("a intrat in search\n");
            // return
            pthread_SEARCH(new_socket);
        }
        else if (strcmp(split[0], "EXIT") == 0 || strcmp(split[0], "exit") == 0){
            printf("Client %d disconnected\n", counter);
            char *buffy = (char*)malloc(32 * sizeof(char));
            strcpy(buffy, "Disconnected");

            send(new_socket, buffy, 32, 0);
            shutdown(new_socket, SHUT_RDWR);
            if (counter == 0){
                counter = 0;
            }else
            counter--;

            pthread_exit(&tid);
        }
        else
            return NULL;
    }
}

int main(int argc, char const *argv[])
{
    set_signals();
    set_signals2();
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    for (int i = 0; i < num_files; i++)
    {
        files[i] = NULL;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    my_fileStruct = (mostRecentWords_T *)malloc(number_of_readFiles * sizeof(mostRecentWords_T));
    
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&th_id_spin, NULL, thread_spinning_func, NULL);

    pthread_mutex_lock(&mutex);
    condition_spin = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    //pthread_join(th_id_spin, NULL);
    //pthread_join(th_id_spin, NULL);
    for (int i = 0; i < number_of_files; i++)
    {
        printf("%d-%s\n", i, files[i]);
    }

    // pthread_join(th_id_spin, NULL);
    //   Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server On\n");
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        
        }
        counter++;

        pthread_mutex_lock(&mutex_signal);
        signal_condition = 0;
        pthread_mutex_unlock(&mutex_signal);
        
        printf("Client %d connected\n", counter);
        pthread_create(&tid, NULL, pthread_exec_func, (void *)&new_socket);
        printf("Thread %d created\n", counter);
        memset(buffer, 0, sizeof(buffer));
    }
    close(new_socket);
    shutdown(server_fd, SHUT_RDWR);
    return 0;
}