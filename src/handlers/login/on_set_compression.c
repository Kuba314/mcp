#include <stdlib.h>
#include <stdbool.h>

#include "unionstream.h"
#include "debug.h"

extern size_t g_compression_threshold;

int on_set_compression(unionstream_t *stream)
{
    int32_t tmp;
    if(stream_read_varint(stream, &tmp)) {
        return 1;
    }
    g_compression_threshold = (size_t) tmp;

    stream->is_compressed = true;
    debug("compression", "threshold set to %ld", g_compression_threshold);
    return 0;
}
