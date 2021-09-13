#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <zlib.h>

#include "_string.h"

typedef struct {
    union {
        struct {
            z_stream infstream;
            void *data;
        };
        int sockfd;
    };
    size_t length;
    size_t offset;
    bool is_compressed;
} unionstream_t;

int stream_read(unionstream_t *stream, uint8_t *dst, size_t size);
void stream_free(unionstream_t *stream);

int stream_read_string(unionstream_t *stream, string_t *str);
