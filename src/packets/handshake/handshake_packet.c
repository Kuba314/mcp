#include "packet_handler.h"
#include "unionstream.h"
#include "sockbuff.h"
#include "packets.h"

int send_Handshake(unionstream_t *stream, int32_t proto_version, uint8_t next_state) {
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 0);
    err |= sockbuff_write_varint(buff, proto_version);
    // err |= sockbuff_write_c_string(buff, "mc.hypixel.net", 14);
    err |= sockbuff_write_c_string(buff, "", 0);
    err |= sockbuff_write_short(buff, 25565);
    err |= sockbuff_write_byte(buff, next_state);
    if(err) {
        return err;
    }

    if(next_state == 1) {
        g_connection_state = CONN_STATE_STATUS;
    } else if(next_state == 2) {
        g_connection_state = CONN_STATE_LOGIN;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
