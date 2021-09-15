#include "packets.h"

int send_LoginStart(unionstream_t *stream, const char *username, size_t username_length)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 0);
    err |= sockbuff_write_c_string(buff, username, username_length);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
