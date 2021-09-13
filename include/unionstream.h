#pragma once

#include <openssl/evp.h>
#include <stdbool.h>
#include <stdint.h>

#include "_string.h"

typedef struct {
    int sockfd;

    uint8_t *data;
    size_t length;
    size_t offset;

    EVP_CIPHER_CTX *en_ctx;
    EVP_CIPHER_CTX *de_ctx;

    bool is_compressed;
    bool is_encrypted;
} unionstream_t;

int stream_read(unionstream_t *stream, uint8_t *dst, size_t size);
void stream_free(unionstream_t *stream);

int stream_read_varint(unionstream_t *stream, int32_t *value);
int stream_read_string(unionstream_t *stream, string_t *str);

int stream_load_packet(unionstream_t *stream);
int stream_write_packet(unionstream_t *stream, const uint8_t *buff, size_t length);
