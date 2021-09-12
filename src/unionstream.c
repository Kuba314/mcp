#include "unionstream.h"

#include <unistd.h>
#include <stdint.h>

int stream_read(unionstream_t *stream, uint8_t *dst, size_t size)
{
    if(size > stream->length - stream->offset) {
        fprintf(stderr, "stream_read: Requested size is larger than data left\n");
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

void stream_free(unionstream_t *stream) {
    if(stream->is_compressed) {
        inflateEnd(&stream->infstream);
        free(stream->data);
    }
}
