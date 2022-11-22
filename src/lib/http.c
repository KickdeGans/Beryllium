#include "http.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char* http_request(char* header)
{
    struct addrinfo hints, *res;
    int sockfd;
    
    char *buf = malloc(32768);
    int byte_count;
    
    memset(&hints, 0,sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("example.com", "80", &hints, &res);

    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

    connect(sockfd,res->ai_addr,res->ai_addrlen);
    send(sockfd,header,strlen(header),0);

    byte_count = recv(sockfd,buf,sizeof(buf),0);

    printf("%i\n", byte_count);
    printf("%s\n", buf);

    return buf;
}