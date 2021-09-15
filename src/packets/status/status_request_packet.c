#include "unionstream.h"
#include "sockbuff.h"
#include "packets.h"

int send_StatusRequest(unionstream_t *stream)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_varint(buff, 0);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
