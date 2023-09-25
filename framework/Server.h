#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>

#include "logger/Logger.h"
#include "request/Request.h"
#include "response/Response.h"

typedef int bool;
#define true 1
#define false 0

#ifndef PORT
#define PORT 8080 //Port users will be connecting to
#endif
// Define the ThreadArgs structure globally
struct ThreadArgs {
    int sockfd;
    struct sockaddr_in *client_addr_ptr;
};


void handle_connection(void *args);  //Handle web requests
void fatal(char *a);

int send_string(int sockfd, unsigned char *buffer);
int recv_line(int sockfd, unsigned char *dest_buffer);

void handle404(int sockfd);


// Implement this methods
Response *handle_api(Request *request);
char *get_path();