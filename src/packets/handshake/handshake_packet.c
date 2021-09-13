#include "sockbuff.h"
#include "packet_handler.h"

int send_Handshake(int sockfd, int32_t proto_version, uint8_t next_state) {
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 0);
    err |= sockbuff_write_varint(buff, proto_version);
    err |= sockbuff_write_string(buff, "mc.hypixel.net\x00\x46ML\x00", 19);
    err |= sockbuff_write_short(buff, 25565);
    err |= sockbuff_write_byte(buff, next_state);
    if(err) {
        return err;
    }

    if(next_state == 1) {
        connection_state = CONN_STATE_STATUS;
    } else if(next_state == 2) {
        connection_state = CONN_STATE_LOGIN;
    }

    sockbuff_dumpto(buff, sockfd);
    return 0;
}
