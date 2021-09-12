#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdbool.h>

#define SOCK2STREAM(sockfd) ((unionstream_t) {.is_compressed = 0, .sockfd = sockfd})

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
