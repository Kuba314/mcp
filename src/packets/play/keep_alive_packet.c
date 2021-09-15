#include "packets.h"
#include "sockbuff.h"
#include "unionstream.h"

int send_Keep_alive(unionstream_t *stream, int64_t keep_alive_id)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 0);
    err |= sockbuff_write_varint(buff, keep_alive_id);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
