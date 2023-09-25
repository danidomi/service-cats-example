#include "Server.h"



int main(void) {
    int sockfd, *new_sockfd, yes = 1;
    struct sockaddr_in host_addr, client_addr;      //My address information
    socklen_t sin_size;

    logMessage(INFO,"Accepting web requests on port %d\n", PORT);

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        fatal("in socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        fatal("setting socket option SO_REUSEADDR\n");

    host_addr.sin_family = AF_INET;     //Host byte order
    host_addr.sin_port = htons(PORT);       //Short, newtork byte order
    host_addr.sin_addr.s_addr = INADDR_ANY; //Automatically fill with my IP
    memset(&(host_addr.sin_zero), '\0', 8); //Zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *) &host_addr, sizeof(struct sockaddr)) == -1)
        fatal("binding to socket");

    if (listen(sockfd, 20) == -1)
        fatal("listening on socket");

    while (1) {  //accept loop
        sin_size = sizeof(struct sockaddr_in);
        new_sockfd = (int *) malloc(sizeof(int)); // Allocate memory for the new socket descriptor
        if (new_sockfd == NULL)
            fatal("malloc");

        *new_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size);
        if (*new_sockfd == -1) {
            free(new_sockfd);
            fatal("accepting connection");
        }

        // Create a new thread and pass sockfd and client_addr_ptr as arguments
        pthread_t thread;

        struct ThreadArgs *args = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        if (args == NULL) {
            free(new_sockfd);
            fatal("malloc");
        }
        args->sockfd = *new_sockfd;
        args->client_addr_ptr = &client_addr;
        if (pthread_create(&thread, NULL, (void *(*)(void *)) handle_connection, args) != 0) {
            free(new_sockfd);
            fatal("pthread_create");
        }

        // Detach the thread to allow it to clean up automatically
        pthread_detach(thread);
    }//Close while
    return 0;
}//Close main

/***************************************************
This funtion handles the connection on the passed socket
from the passed client address. The connection is processed
as a web request, and this function replies over the
connected socket. Finally, the passed socket is closed
at the end of the function.
****************************************************/
void handle_connection(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *) arg;
    int sockfd = args->sockfd;
    struct sockaddr_in *client_addr_ptr = args->client_addr_ptr;
    unsigned char *ptr, *resource;
    Request *request;
    char *req;
    char *requestPlain;
    int fd, length;

    resource = malloc(50);
    req = malloc(4000);
    length = recv_line(sockfd, req);
    printf("%s\n\n", req);
    request = parseRequest(req);
    if (request != NULL) {
        printf("Method: %s\n", request->method);
        printf("Path: %s\n", request->path);
        printf("Query Parameters:\n");
        for (size_t i = 0; i < request->queryParamsCount; i++) {
            printf("\tKey: %s, Value: %s\n", request->queryParams[i].key, request->queryParams[i].value);
        }
    } else {
        printf("Failed to parse request.\n");
    }
    printf("Got request from %s:%d \"%s\"\n", inet_ntoa(client_addr_ptr->sin_addr),
           ntohs(client_addr_ptr->sin_port), req);
    ptr = strstr(req, "HTTP/");     //Search for valid looking request
    if (ptr == NULL) {       //The this isnt a valid HTTP
        printf("NOT HTTP!\n");
    } else if (strcmp(request->path, "/favicon.ico") == 0) {
        handle404(sockfd);
        shutdown(sockfd, SHUT_RDWR);
        return;
    } else {
        if (strcmp(request->path, get_path()) == 0) {
            Response *response = handle_api(request);
            if (response == NULL) {
                printf("RESPONSE IS NULL");
                handle404(sockfd);
                shutdown(sockfd, SHUT_RDWR);
                return;
            }

            char *http = malloc(9 + strlen(response->status_code) * sizeof(char));
            char *header = malloc(strlen(response->headers[0]) * response->headersCount * sizeof(char));
            char data[1000];
            //char *data = malloc(strlen(response->data) * sizeof(char));
            sprintf(http, "HTTP/1.0 %s\r\n", response->status_code);
            if (response->headersCount > 0) {
                for (int i = 0; i < response->headersCount; i++) {
                    printf("%d\n", i);
                    if (i != (response->headersCount - 1)) {
                        sprintf(header, "%s\r\n", response->headers[i]);
                    } else {
                        sprintf(header, "%s\r\n\r\n", response->headers[i]);
                    }
                }
            }
            printf("data: %s\n", response->data);
            sprintf(data, "%s\r\n", response->data);
            send_string(sockfd, http);
            send_string(sockfd, header);
            send_string(sockfd, data);
            free(http);
            free(header);
            //free(data);
        } else {
            handle404(sockfd);
        } //Close else printf 200 block
    }//End if block for valid HTTP
    shutdown(sockfd, SHUT_RDWR);    //close socket ever so gingerly
}//Close void handle_connection


void handle404(int sockfd) {
    send_string(sockfd, "HTTP/1.0 404 Not Found\r\n");
    send_string(sockfd, "Server: Tiny webserver \r\n\r\n");
    send_string(sockfd, "<html><head><title>404 not found</title></head>");
    send_string(sockfd, "<body><h1>URL not found</h1></body></html>\r\n");
}

/*******************************************************
This function accepts an open file descriptor and returns
the size of the associated file. returns -1 on failure
********************************************************/
int get_file_size(int fd) {
    struct stat stat_struct;

    if (fstat(fd, &stat_struct) == -1)
        return -1;
    return (int) stat_struct.st_size;
}// close get_file_size

// Definition of function fatal().
void fatal(char *a) {
    printf("The string of (a) is: %s\n", a);
}

/* This function accepts a socket FD and a ptr to the null terminated
 * string to send.  The function will make sure all the bytes of the
 * string are sent.  Returns 1 on success and 0 on failure.
 */
int send_string(int sockfd, unsigned char *buffer) {
    int sent_bytes, bytes_to_send;
    bytes_to_send = strlen(buffer);
    while (bytes_to_send > 0) {
        sent_bytes = send(sockfd, buffer, bytes_to_send, 0);
        if (sent_bytes == -1)
            return 0; // return 0 on send error
        bytes_to_send -= sent_bytes;
        buffer += sent_bytes;
    }
    return 1; // return 1 on success
}

int recv_line(int sockfd, unsigned char *dest_buffer) {
#define EOL "\r\n" // End-Of-Line byte sequence
#define EOL_SIZE 2
    unsigned char *ptr;
    int eol_matched = 0;

    ptr = dest_buffer;
    while (recv(sockfd, ptr, 1, 0) == 1) { // read a single byte
        if (*ptr == EOL[eol_matched]) { // does this byte match terminator
            eol_matched++;
            if (eol_matched == EOL_SIZE) { // if all bytes match terminator,
                *(ptr + 1 - EOL_SIZE) = '\0'; // terminate the string
                return strlen(dest_buffer); // return bytes recevied
            }
        } else {
            eol_matched = 0;
        }
        ptr++; // increment the pointer to the next byter;
    }
    return 0; // didn't find the end of line characters
}