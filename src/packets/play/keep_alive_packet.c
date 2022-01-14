#include "packets.h"

int send_KeepAlive(stream_t *stream, int32_t keep_alive_id)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_byte(buff, 0);
    err |= buffer_write_varint(buff, keep_alive_id);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
