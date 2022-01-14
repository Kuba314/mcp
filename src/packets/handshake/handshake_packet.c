#include "packets.h"
#include "packet_handler.h"

int send_Handshake(stream_t *stream, int32_t proto_version, uint8_t next_state)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_byte(buff, 0);
    err |= buffer_write_varint(buff, proto_version);
    err |= buffer_write_c_string(buff, "mc.hypixel.net", 14);
    err |= buffer_write_short(buff, 25565);
    err |= buffer_write_byte(buff, next_state);
    if(err) {
        return err;
    }

    if(next_state == 1) {
        g_connection_state = CONN_STATE_STATUS;
    } else if(next_state == 2) {
        g_connection_state = CONN_STATE_LOGIN;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
