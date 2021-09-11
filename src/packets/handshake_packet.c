#include "sockbuff.h"
#include "check.h"

#include <sys/socket.h>
#include <string.h>

int send_Handshake(int sockfd, int32_t proto_version, uint8_t next_state)
{
    sockbuff_t *buff = sockbuff_create();
    FAIL_ON_WITH(NULL, 1, buff);

    FINE_ON_WITH(0, 1, sockbuff_write_byte(buff, 0));
    FINE_ON_WITH(0, 1, sockbuff_write_varint(buff, proto_version));
    FINE_ON_WITH(0, 1, sockbuff_write_string(buff, "mc.hypixel.net\x00\x46ML\x00", 19));
    FINE_ON_WITH(0, 1, sockbuff_write_short(buff, 25565));
    FINE_ON_WITH(0, 1, sockbuff_write_byte(buff, next_state));
    sockbuff_dumpto(buff, sockfd);

    return 0;
}

int send_StatusRequest(int sockfd)
{
    send(sockfd, "\x01\x00", 2, 0);
    return 0;
}

int send_LoginStart(int sockfd, const char *username)
{
    size_t name_length = strlen(username);

    sockbuff_t *buff = sockbuff_create();
    FAIL_ON_WITH(NULL, 1, buff);

    FINE_ON_WITH(0, 1, sockbuff_write_byte(buff, 0));
    FINE_ON_WITH(0, 1, sockbuff_write_string(buff, username, name_length));
    sockbuff_dumpto(buff, sockfd);

    return 0;
}
