#include <stdlib.h>

#include "unionstream.h"
#include "debug.h"

extern int g_compression_threshold;

int on_set_compression(unionstream_t *stream)
{
    if(stream_read_varint(stream, &g_compression_threshold)) {
        return 1;
    }

    debug("compression", "threshold set to %d", g_compression_threshold);
    return 0;
}
