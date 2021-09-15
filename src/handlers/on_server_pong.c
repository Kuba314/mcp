#include "unionstream.h"
#include "debug.h"

int on_server_pong(unionstream_t *stream)
{
    // should be big endian, but whatever
    // just needed to consume 8 bytes
    // not like this is used anywhere
    int64_t payload;
    if(stream_read(stream, &payload, 8)) {
        return 1;
    }
    return 0;
}
