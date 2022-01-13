#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <semaphore.h>

#include "position.h"
#include "_string.h"

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
} unionstream_t;

int stream_create(int sockfd, unionstream_t **stream);
void stream_print_rest(unionstream_t *stream);
void stream_free_data(unionstream_t *stream);
void stream_free(unionstream_t *stream);

int stream_load_packet(unionstream_t *stream);
int stream_write_packet(unionstream_t *stream, const void *buff, size_t length);

int stream_read_directly(unionstream_t *stream, void *dst, size_t length);
int stream_read_varint_directly(unionstream_t *stream, int32_t *value);

int stream_read(unionstream_t *stream, void *dst, size_t size);
int stream_read_rev(unionstream_t *stream, void *dst, size_t length);

inline int stream_read_bool(unionstream_t *stream, bool *value) { return stream_read_rev(stream, value, 1); }
inline int stream_read_byte(unionstream_t *stream, int8_t *value) { return stream_read_rev(stream, value, 1); }
inline int stream_read_short(unionstream_t *stream, int16_t *value) { return stream_read_rev(stream, value, 2); }
inline int stream_read_int(unionstream_t *stream, int32_t *value) { return stream_read_rev(stream, value, 4); }
inline int stream_read_long(unionstream_t *stream, int64_t *value) { return stream_read_rev(stream, value, 8); }

inline int stream_read_ubyte(unionstream_t *stream, uint8_t *value) { return stream_read_rev(stream, value, 1); }
inline int stream_read_ushort(unionstream_t *stream, uint16_t *value) { return stream_read_rev(stream, value, 2); }
inline int stream_read_float(unionstream_t *stream, float *value) { return stream_read_rev(stream, value, 4); }
inline int stream_read_double(unionstream_t *stream, double *value) { return stream_read_rev(stream, value, 8); }

string_t *stream_read_string(unionstream_t *stream);

int stream_read_varint(unionstream_t *stream, int32_t *value);
int stream_read_position(unionstream_t *stream, position_t *position);
