#pragma once

#include <openssl/evp.h>
#include <stdint.h>
#include <stdbool.h>

#include "_string.h"

typedef struct {
    int sockfd;

    char *data;
    size_t length;
    size_t offset;

    EVP_CIPHER_CTX *en_ctx;
    EVP_CIPHER_CTX *de_ctx;

    bool is_compressed;
    bool is_encrypted;
} unionstream_t;

int stream_load_packet(unionstream_t *stream);

int stream_read(unionstream_t *stream, void *dst, size_t size);
int stream_read_varint(unionstream_t *stream, int32_t *value);
string_t *stream_read_string(unionstream_t *stream);

int stream_write_packet(unionstream_t *stream, const char *buff,
                        size_t length);
void stream_free(unionstream_t *stream);
