#pragma once

#include <openssl/evp.h>
#include <stdint.h>
#include <stdbool.h>

#include "position.h"
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

int stream_read_directly(unionstream_t *stream, void *dst, size_t length);
int stream_read_varint_directly(unionstream_t *stream, int32_t *value);

int stream_read(unionstream_t *stream, void *dst, size_t size);
int stream_read_rev(unionstream_t *stream, void *dst, size_t length);
int stream_read_varint(unionstream_t *stream, int32_t *value);
int stream_read_int(unionstream_t *stream, int *value);
int stream_read_byte(unionstream_t *stream, uint8_t *value);
int stream_read_float(unionstream_t *stream, float *value);
int stream_read_position(unionstream_t *stream, position_t *position);
string_t *stream_read_string(unionstream_t *stream);

int stream_write_packet(unionstream_t *stream, const void *buff,
                        size_t length);
void stream_free(unionstream_t *stream);
