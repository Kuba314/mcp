#include "packets.h"

int send_EncryptionResponse(stream_t *stream, string_t *enc_aes_key, string_t *enc_verify_token)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_varint(buff, 1);
    err |= buffer_write_string(buff, enc_aes_key);
    err |= buffer_write_string(buff, enc_verify_token);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
