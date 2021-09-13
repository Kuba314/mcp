#include <sys/socket.h>

int send_StatusRequest(int sockfd) {
    return send(sockfd, "\x01\x00", 2, 0);
}
