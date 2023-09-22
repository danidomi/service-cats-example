#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <libc.h>
#include <ctype.h>
#include "Server.h"

#define PORT 8080 //Port users will be connecting to
#define WEBROOT "webroot"   //Webservers root directory

void handle_connection(int, struct sockaddr_in *);  //Handle web requests
int get_file_size(int);     //Returns file size of open file descriptor
void fatal(char *a);
int send_string(int sockfd, unsigned char *buffer);
int recv_line(int sockfd, unsigned char *dest_buffer);
char *trimwhitespace(char *str);


struct Request {
    char* method;
};
int main(void) {
    int sockfd, new_sockfd, yes = 1;
    struct sockaddr_in host_addr, client_addr;      //My address information
    socklen_t sin_size;

    printf("Accepting web requests on port %d\n", PORT);

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
        new_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &sin_size);
        if (new_sockfd == -1)
            fatal("accepting connection");
        handle_connection(new_sockfd, &client_addr);
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
void handle_connection(int sockfd, struct sockaddr_in *client_addr_ptr) {
    unsigned char *ptr, *resource;
    Request *request;
    int fd, length;

    request = malloc(sizeof (Request));
    resource = malloc(50);
    length = recv_line(sockfd, request);

    printf("Got request from %s:%d \"\%s\"\n", inet_ntoa(client_addr_ptr->sin_addr),
           ntohs(client_addr_ptr->sin_port), request);

    ptr = strstr(request, "HTTP/");     //Search for valid looking request
    if (ptr == NULL) {       //The this isnt a valid HTTP
        printf("NOT HTTP!\n");
    } else {
        *ptr = 0;   //Terminate the buffer at the end of the URL
        ptr = NULL;
        if (strncmp(request, "GET", 3) == 0){
            ptr = (unsigned char *) (request + 4);  //ptr is URL
            request->method = "GET";
        }
        else if (strncmp(request, "HEAD", 4) == 0) {
            ptr = (unsigned char *) (request + 5);  //ptr is URL
            request->method = "HEAD";
        }else if (strncmp(request, "POST", 4) == 0) {
            ptr = (unsigned char *) (request + 5);  //ptr is URL
            request->method = "POST";
        }
        ptr = "api";
        printf("%s\n", request->method);
        printf("%s\n", ptr);
        if (ptr == NULL) {       //Then this is not a recognized request
            printf("\tUNKNOWN REQUEST!\n");
        } else {        //Valid request, with ptr ponting to the resource name
            if (ptr[strlen(ptr) - 1] == '/') //For resources ending with '/'
                strcat(ptr, "index.html");  //ad index.html to end.
            strcpy(resource, WEBROOT);  //Begin resource with WEBROOT
            strcat(resource, ptr);          //and join it with resource path
            printf("%s\n", resource);
            printf("%s\n", ptr);
            resource = trimwhitespace(resource);
            fd = open(resource, O_RDONLY, 0);   //Try to open the file
            printf("\tOpening \'%s \'\t", resource);
            if ((strncmp(ptr, "api", 3) == 0)) {
                Response * response = handleApi(request);
                char http [50];
                char header [50];
                char data [50];
                sprintf(http, "HTTP/1.0 %s\r\n", response->status_code);
                sprintf(header, "Content-Type: %s\r\n\r\n", response->content_type);
                sprintf(data, "%s\r\n", response->response);
                send_string(sockfd, http);
                send_string(sockfd, header);
                send_string(sockfd, data);
            }
            else if (fd == -1) {      //If file is not found
                printf(" 404 Not Found\n");
                send_string(sockfd, "HTTP/1.0 404 Not Found\r\n");
                send_string(sockfd, "Server: Tiny webserver \r\n\r\n");
                send_string(sockfd, "<html><head><title>404 not found</title></head>");
                send_string(sockfd, "<body><h1>URL not found</h1></body></html>\r\n");
            } else {
                //Otherwise serve up the file
                printf(" 200 OK\n");
                send_string(sockfd, "HTTP/1.0 200 OK\r\n");
                send_string(sockfd, "Server: Tiny webserver\r\n\r\n");
                if (ptr == request + 4) {    //Then this is a GET request
                    if ((length = get_file_size(fd)) == -1)
                        fatal("getting resource file size");
                    if ((ptr = (unsigned char *) malloc(length)) == NULL)
                        fatal("allocating memory for reading resource");
                    read(fd, ptr, length);  //Read file into memory
                    send(sockfd, ptr, length, 0);   //Send it to socket
                    free(ptr);      //Free file memory
                }//Close if
                close(fd);      //Close file
            } //Close else printf 200 block
        }// close preceding else to above else statement
    }//End if block for valid HTTP
    shutdown(sockfd, SHUT_RDWR);    //close socket ever so gingerly
}//Close void handle_connection

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

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}