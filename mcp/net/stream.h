#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <semaphore.h>

#include "utils/buffer.h"
#include "data/position.h"
#include "utils/dynstring.h"

typedef struct {
    int sockfd;
    sem_t lock;

    uint8_t *data;
    size_t length;
    size_t offset;

    EVP_CIPHER_CTX *en_ctx;
    EVP_CIPHER_CTX *de_ctx;

    bool is_compressed;
    bool is_encrypted;
} stream_t;

int stream_create(int sockfd, stream_t **stream);
void stream_free_data(stream_t *stream);
void stream_free(stream_t *stream);

int stream_load_packet(stream_t *stream);
int stream_write_packet(stream_t *stream, buffer_t *buff);

int stream_read_directly(stream_t *stream, void *dst, size_t length);
int stream_read_varint_directly(stream_t *stream, int32_t *value);

int stream_read(stream_t *stream, void *dst, size_t size);
int stream_read_rev(stream_t *stream, void *dst, size_t length);

inline int stream_read_bool(stream_t *stream, bool *value)
{
    return stream_read_rev(stream, value, 1);
}
inline int stream_read_byte(stream_t *stream, int8_t *value)
{
    return stream_read_rev(stream, value, 1);
}
inline int stream_read_short(stream_t *stream, int16_t *value)
{
    return stream_read_rev(stream, value, 2);
}
inline int stream_read_int(stream_t *stream, int32_t *value)
{
    return stream_read_rev(stream, value, 4);
}
inline int stream_read_long(stream_t *stream, int64_t *value)
{
    return stream_read_rev(stream, value, 8);
}

inline int stream_read_ubyte(stream_t *stream, uint8_t *value)
{
    return stream_read_rev(stream, value, 1);
}
inline int stream_read_ushort(stream_t *stream, uint16_t *value)
{
    return stream_read_rev(stream, value, 2);
}
inline int stream_read_float(stream_t *stream, float *value)
{
    return stream_read_rev(stream, value, 4);
}
inline int stream_read_double(stream_t *stream, double *value)
{
    return stream_read_rev(stream, value, 8);
}

string_t *stream_read_string(stream_t *stream);

int stream_read_varint(stream_t *stream, int32_t *value);
int stream_read_position(stream_t *stream, position_t *position);
