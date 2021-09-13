#include "unionstream.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "varint.h"

int stream_read(unionstream_t *stream, uint8_t *dst, size_t size) {
    if(size <= 0) {
        return 0;
    }
    if(size > stream->length - stream->offset) {
        fprintf(stderr,
                "stream_read: Requested size is larger than data left\n");
        return 1;
    }

    if(stream->is_compressed) {
        stream->infstream.avail_out = size;
        stream->infstream.next_out = (Bytef *) dst;

        return inflate(&stream->infstream, Z_SYNC_FLUSH);
    } else {
        ssize_t n_read_total = 0;
        ssize_t n_read = 0;
        while(n_read_total < (ssize_t) size) {
            n_read = read(stream->sockfd, dst + n_read, size);
            if(n_read < 0) {
                return 1;
            }
            n_read_total += n_read;
        }
        stream->offset += size;
        return 0;
    }
}

int stream_read_string(unionstream_t *stream, string_t *str) {
    int32_t length;
    read_varint(stream, &length, NULL);

    if(length < 0) {
        return 1;
    } else if(length == 0) {
        *str = (string_t){ 0 };
    } else {
        str->s = malloc(length);
        if(str->s == NULL) {
            perror("stream_read_string");
            return 1;
        }
        str->length = length;
        stream_read(stream, str->s, length);
    }
    return 0;
}

void stream_free(unionstream_t *stream) {
    if(stream->is_compressed) {
        inflateEnd(&stream->infstream);
        free(stream->data);
    }
}
