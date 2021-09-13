#include <string.h>

#include "_string.h"
#include "debug.h"
#include "sockbuff.h"
#include "unionstream.h"

int send_EncryptionResponse(unionstream_t *stream, size_t secret_length,
                            const char *secret, size_t token_length,
                            const char *token) {

    sockbuff_t *buff = sockbuff_create();
    if(buff == 0) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_varint(buff, 1);
    err |= sockbuff_write_string(buff, secret, secret_length);
    err |= sockbuff_write_string(buff, token, token_length);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
