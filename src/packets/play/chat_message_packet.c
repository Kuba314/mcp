#include "packets.h"

int send_ChatMessage(unionstream_t *stream, const char *message, size_t message_len)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 1);
    err |= sockbuff_write_c_string(buff, message, message_len);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
