#include "connection.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int mc_connect(const char *ip, uint16_t port) {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        perror("mc_connect: socket creation");
        return sockfd;
    }

    struct sockaddr_in serv_addr = { .sin_family = AF_INET,
                                  .sin_port = htons(port) };

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr) != 1) {
        perror("mc_connect: inet_pton");
        return -1;
    }

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("mc_connect: connect");
        return -1;
    }

    return sockfd;
}
