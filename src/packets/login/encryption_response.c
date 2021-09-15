#include "packets.h"

int send_EncryptionResponse(unionstream_t *stream, string_t *enc_aes_key, string_t *enc_verify_token)
{
    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_varint(buff, 1);
    err |= sockbuff_write_string(buff, enc_aes_key);
    err |= sockbuff_write_string(buff, enc_verify_token);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
