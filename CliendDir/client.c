// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
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
#include <sys/stat.h>
#include <dirent.h>

#define PORT 8080
#define num_files 1024
#define BUFSIZE 1024

#define protocol_chr "!"

int nr_of_files = 0;

int sock = 0;
int client_fd = 0;
int valread = 0;

char **get_string_splitted(char *input, const char *chr, int size)
{
    char **toRet = (char **)malloc(size * sizeof(char *));
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

char *get_status(char *input, const char *chr, int size)
{
    char **splitted = get_string_splitted(input, chr, size);
    return splitted[0];
}

int get_number_of_files_SEARCH(char *input, const char *chr, int size){
 
    char **splitted = get_string_splitted(input, chr, size);
       //printf("%s-%d\n", splitted[0], atoi(splitted[1]));
    return atoi(splitted[1]);
}

char **get_fileList(char *input, const char *chr, int size)
{
    printf("%s\n", input);
    char **splitted = get_string_splitted(input, chr, size);
    char *files = (char *)malloc(num_files * sizeof(char));
    files = NULL;
    files = strdup(splitted[2]);
    char **files_splitted = get_string_splitted(files, "\\0", size);

    nr_of_files = atoi(splitted[3]);

    return files_splitted;
}

char **get_fileLineList(char *input, const char *chr, int size)
{
    char **splitted = get_string_splitted(input, chr, size);
    char *files = (char *)malloc(num_files * sizeof(char));
    files = NULL;
    files = strdup(splitted[2]);
    nr_of_files = atoi(splitted[3]);
    char **files_splitted = get_string_splitted(files, "\n", size);

    return files_splitted;
}

int get_size_of_payload(char *input, const char *chr, int size)
{
    char **splitted = get_string_splitted(input, chr, size);
    return atoi(splitted[1]);
}

char *get_to_send_string(char *operatie, char *nume_fisier)
{
    char *final = (char *)malloc(32 * sizeof(char));
    if (strcmp(operatie, "GET") == 0 || strcmp(operatie, "get") == 0)
    {
        strcpy(final, operatie);
        strcat(final, "!");

        char result_integer[50];
        sprintf(result_integer, "%ld", sizeof(nume_fisier));

        strcat(final, result_integer);
        strcat(final, "!");
        strcat(final, nume_fisier);
        return final;
    }
    else if (strcmp(operatie, "PUT") == 0 || strcmp(operatie, "put") == 0)
    {
        strcpy(final, operatie);
        strcat(final, "!");

        char result_integer[50];
        sprintf(result_integer, "%ld", sizeof(nume_fisier));

        strcat(final, result_integer);
        strcat(final, "!");
        strcat(final, nume_fisier);
        return final;
    }
    else if (strcmp(operatie, "DELETE") == 0 || strcmp(operatie, "delete") == 0)
    {
        strcpy(final, operatie);
        strcat(final, "!");

        char result_integer[50];
        sprintf(result_integer, "%ld", sizeof(nume_fisier));

        strcat(final, result_integer);
        strcat(final, "!");
        strcat(final, nume_fisier);
        return final;
    }
    else if (strcmp(operatie, "UPDATE") == 0 || strcmp(operatie, "update") == 0)
    {
        strcpy(final, operatie);
        strcat(final, "!");

        char result_integer[50];
        sprintf(result_integer, "%ld", sizeof(nume_fisier));

        strcat(final, result_integer);
        strcat(final, "!");
        strcat(final, nume_fisier);
        return final;
    }
    else if (strcmp(operatie, "SEARCH") == 0 || strcmp(operatie, "search") == 0)
    {
        strcpy(final, operatie);
        strcat(final, "!");

        char result_integer[50];
        sprintf(result_integer, "%ld", sizeof(nume_fisier));

        strcat(final, result_integer);
        strcat(final, "!");
        strcat(final, nume_fisier);
        return final;
    }  
    else if (strcmp(operatie, "LIST") == 0 || strcmp(operatie, "list") == 0)
    {
        return operatie;
    }
    else if (strcmp(operatie, "EXIT") == 0 || strcmp(operatie, "exit") == 0){
        return operatie;
    }
}

void signal_handler(){
    shutdown(sock, SHUT_RDWR);
    shutdown(client_fd, SHUT_RDWR);
    printf("Client will shut down\n");
    exit(EXIT_SUCCESS);
}

void set_signal(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));

    sa.sa_flags = SA_SIGINFO ;
    sa.sa_handler = signal_handler;
    sigaction(SIGSEGV, &sa, NULL);
}

int main(int argc, char const *argv[])
{
    set_signal();
    struct sockaddr_in serv_addr;
    char *input = (char *)malloc(2048 * sizeof(char));
    char buffer[BUFSIZE] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.1.1", &serv_addr.sin_addr) <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int sic = 0;
    while (1)
    {
        //printf("checkk\n");

        printf("LIST/list: list files-size from server dir\n");
        printf("GET/get <filename>: get <filename> file from server dir\n");
        printf("PUT/put <filename>: put <filename> file in server dir\n");
        printf("DELETE/delete <filename>: delete <filename> file from server dir\n");
        printf("UPDATE/update <filename>: update content of <filename> file in server dir\n");
        printf("SEARCH/search <word>: search content of <word> in files from server dir\n");
        printf("EXIT/exit: disconnect client\n");

        int error = 0;
        socklen_t len = sizeof (error);
        int retval = getsockopt (sock, SOL_SOCKET, SO_ERROR, &error, &len);

        printf("> ");  
        memset(input, 0, sizeof(input));
        //printf("checkkk\n");
        char auxx[2] = {0};
        //fseek(stdin,0,SEEK_END);
        fgets(input, 31, stdin);

        if (error != 0) {
        /* socket has a non zero error status */
            fprintf(stderr, "socket error: %s\n", strerror(error));
            close(client_fd);
            return 0;
        }

        if (strcmp(input,"\n") == 0){
            fgets(input, 31, stdin);
        }
        //printf("checkkkk\n");
        char *rest = strdup(input);
        //printf("checkkkkk\n");
        input = strtok_r(input, "\n", &rest);
        //printf("checkkkkkk\n");
        char **aux_input = get_string_splitted(input, " ", num_files);
        //printf("checkkkkkkk\n");
        strcpy(input, get_to_send_string(aux_input[0], aux_input[1]));
        //printf("checkkkkkkkk\n");
        if (strcmp(input, "LIST") == 0 || strcmp(input, "list") == 0)
        {
            memset(buffer, 0, sizeof(buffer));
            send(sock, input, strlen(input), 0);
            valread = recv(sock, buffer, 5120, 0);
            char *aux_buffer_0 = strdup(buffer);
            char *aux_buffer_1 = strdup(buffer);
            char *aux_buffer_2 = strdup(buffer);
            char *status = get_status(aux_buffer_0, protocol_chr, num_files);
            // printf("1");
            char **filenames = get_fileList(aux_buffer_1, protocol_chr, num_files);
            // printf("2");
            int size = get_size_of_payload(aux_buffer_2, protocol_chr, num_files);
            // printf("3");
            printf("Status: %s\n", status);

            for (int i = 0; i < nr_of_files; i++)
            {
                printf("->%d: %s\n", i + 1, filenames[i]);
            }
        }
        else if (strstr(input, "GET") != NULL || strstr(input, "get") != NULL)
        {
            memset(buffer, 0, sizeof(buffer));
            send(sock, input, strlen(input), 0);
            int out = open(aux_input[1], O_WRONLY | O_CREAT, 0777);
            if (out < 0)
            {
                printf("output file error\n");
                exit(EXIT_FAILURE);
            }
            valread = recv(sock, buffer, BUFSIZE, 0);
            char *auxBuffer1 = strdup(buffer);
            // printf("buf1024: %s\n", buffer);
            char *status = get_status(auxBuffer1, "!", num_files);
            printf("Server sent: %s\n", auxBuffer1);
            char **returned = get_string_splitted(buffer, protocol_chr, num_files);
            if (strcmp(returned[0], "Success") == 0)
            {
                int sizeFile = atoi(returned[1]);
                int bytes_received;
                char reader[sizeFile + 1];
                recv(sock, reader, sizeFile + 1, 0);
                // printf("READER: \n %s", reader);
                write(out, reader, sizeFile);
            }
            else
            {
                printf("Failed\n");
            }
        }
        else if (strstr(input, "PUT") != NULL || strstr(input, "put") != NULL)
        {

            int in_fd = open(aux_input[1], O_RDONLY);
            if (in_fd < 0)
            {
                printf("erorr put-input file\n");
                exit(EXIT_FAILURE);
            }
            else
            {

                struct stat st;
                stat(aux_input[1], &st);
                int sizeFile = st.st_size;

                char result_integer[50];
                sprintf(result_integer, "%d", sizeFile);
                strcat(input, "!");
                strcat(input, result_integer);
                strcat(input, "!");

                send(sock, input, strlen(input), 0);
                ;
                sendfile(sock, in_fd, 0, sizeFile + 1);
                valread = recv(sock, buffer, BUFSIZE, 0);
                printf("Server send: %s\n", buffer);
            }
        }
        else if (strstr(input, "DELETE") != NULL || strstr(input, "delete") != NULL){
            memset(buffer, 0, sizeof(buffer));
            send(sock, input, strlen(input), 0);
            recv(sock, buffer, 32, 0);
            printf("Server send: %s\n", buffer);
        }
        else if (strstr(input, "UPDATE") != NULL || strstr(input, "update") != NULL)
        {
            int read_size = 0;
            int nr_linesModify = 0;
            printf("Enter number of lines: ");
            scanf("%d", &nr_linesModify);
            printf("\nEnter lines:\n");
            char *my_reader = (char *)malloc(4 * BUFSIZE);
            char *string_to_send = (char *)malloc(4 * BUFSIZE * nr_linesModify * sizeof(char));
            memset(string_to_send, 0, sizeof(string_to_send));
            // fflush(stdin);
            char la[10];
            fgets(la, 10, stdin);
            printf("-> ");
            fgets(my_reader, 4 * BUFSIZE, stdin);
            //printf("citit in afara for: %s\n", my_reader);
            read_size += strlen(my_reader);
            strcpy(string_to_send, my_reader);
            memset(my_reader, 0, sizeof(my_reader));
            for (int i = 1; i < nr_linesModify; i++)
            {
                //fgets(la, 10, stdin);
                //fflush(stdin);
                printf("-> ");
                fgets(my_reader, 4 * BUFSIZE, stdin);
                //printf("citit in for: %s\n", my_reader);
                strcat(string_to_send, my_reader);
                read_size += strlen(my_reader);
                memset(my_reader, 0, sizeof(my_reader));
            }
            printf("Start byte: ");
            off_t offByte;
            scanf("%ld", &offByte);
            memset(buffer, 0, sizeof(buffer));

            strcat(input, "!");

            char result_integer[128];
            sprintf(result_integer, "%ld", offByte);

            strcat(input, result_integer);
            strcat(input, "!");
            printf("result_integer: %s---%ld\n", result_integer, offByte);
            char result_integer1[64];
            sprintf(result_integer1, "%d", read_size);
            printf("result_integer1: %s---%d\n", result_integer1, read_size);
            strcat(input, result_integer1);
            printf("input: %s\n", input);
            send(sock, input, BUFSIZE, 0);
            send(sock, string_to_send, read_size + 1, 0);

            recv(sock, buffer, 32, 0);
            printf("Server send: %s\n", buffer);
        }
        else if (strstr(input, "SEARCH") != NULL || strstr(input, "search") != NULL){
            memset(buffer, 0, sizeof(buffer));
            send(sock, input, strlen(input), 0);
            recv(sock, buffer, 64, 0);
            //printf("buffer: %s\n", buffer);
            char *auxBuffer1 = strdup(buffer);
            char *auxBuffer2 = strdup(buffer);
            //printf("dupa strdup\n");
            char *status = get_status(auxBuffer1, "!", num_files);
            printf("status: %s\n", status);
            //printf("1\n");
            if (strcmp(status, "Failed") == 0){

                printf("Server sent: %s\n", status);
            }else if (strcmp(status, "Success") == 0){
                char auxBuffer[BUFSIZE];               
                int r = 0;
                r = recv(sock, auxBuffer, 1024, 0);
                //printf("r = %d\n", r);
                //printf("on success\n");
                printf("Server sent: %s\n", status);
                int nrFiles = get_number_of_files_SEARCH(auxBuffer2, "!", num_files);
                int value = BUFSIZE * nrFiles;

               
                printf("%s\n", auxBuffer);
                char **myRes = get_string_splitted(auxBuffer, "\\0", value);
                printf("List received from server: \n");
                for (int i = 0; i < nrFiles; i++){
                    printf("%d->%s\n", i + 1, myRes[i]);
                }
            }
        }
        else if (strcmp(input, "EXIT") == 0 || strcmp(input, "exit") == 0)
        {
            memset(buffer, 0, sizeof(buffer));
            send(sock, input, strlen(input), 0);
            recv(sock, buffer, 32, 0);
            printf("Server send: %s\n", buffer);
            
            close(client_fd);
            return 0;
        }
    }

    // valread = recv(sock, buffer, 1024, 0);
    // printf("%s\n", buffer);

    // closing the connected socket
    nr_of_files = 0;
    close(client_fd);
    return 0;
}