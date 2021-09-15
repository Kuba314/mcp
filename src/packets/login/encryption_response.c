#include "packets.h"
#include "unionstream.h"
#include "sockbuff.h"
#include "_string.h"

int send_EncryptionResponse(unionstream_t *stream, string_t *secret, string_t *token)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_varint(buff, 1);
    err |= sockbuff_write_string(buff, secret);
    err |= sockbuff_write_string(buff, token);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
