#include <stdio.h>
#include <stdlib.h>

#include "_string.h"
#include "packets.h"
#include "unionstream.h"
#include "varint.h"

int auth_create_encrypted_pair(string_t pubkey, string_t auth_token,
                               string_t *enc_secret, string_t *enc_token);

void on_encryption_request(unionstream_t *stream) {

    string_t server_id;
    stream_read_string(stream, &server_id);

    string_t pubkey;
    stream_read_string(stream, &pubkey);

    string_t verify_token;
    stream_read_string(stream, &verify_token);

    string_t enc_aes_key;
    string_t enc_token;
    auth_create_encrypted_pair(pubkey, verify_token, &enc_aes_key, &enc_token);

    send_EncryptionResponse(stream->sockfd, enc_aes_key.length, enc_aes_key.s,
                            enc_token.length, enc_token.s);
    free(enc_aes_key.s);
    free(enc_token.s);
}
