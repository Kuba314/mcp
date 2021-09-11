#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <sys/socket.h>
#include "stdbool.h"

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
    bool is_compressed;
} unionstream_t;

inline int stream_read(unionstream_t stream, void *dst, size_t size) {
    if(stream.is_compressed) {
        stream.infstream.avail_out = size;
        stream.infstream.next_out = (Bytef *) dst;

        return inflate(&stream.infstream, Z_SYNC_FLUSH);
    } else {
        ssize_t n_read_total = 0;
        ssize_t n_read;
        while(n_read_total < (ssize_t) size) {
            n_read = read(stream.sockfd, dst, size);
            if(n_read < 0) {
                return 1;
            }
            n_read_total += n_read;
        }
        return 0;
    }
}

inline void stream_free(unionstream_t stream) {
    if(stream.is_compressed) {
        inflateEnd(&stream.infstream);
        free(stream.data);
    }
}
