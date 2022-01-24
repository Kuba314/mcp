#include "net/packets.h"

int send_StatusRequest(stream_t *stream)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_varint(buff, 0);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
