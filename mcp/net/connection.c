#include "net/connection.h"

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "debug.h"

int connect_to_server(const char *ip, uint16_t port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        error_desc("socket", "could not create socket");
        return sockfd;
    }

    struct sockaddr_in serv_addr = { .sin_family = AF_INET, .sin_port = htons(port) };

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) != 1) {
        error_desc("socket", "could not convert ip to address");
        return -1;
    }

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error_desc("socket", "could not connect");
        return -1;
    }

    return sockfd;
}
