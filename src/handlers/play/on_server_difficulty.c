#include "packet_handler.h"

int on_server_difficulty(unionstream_t *stream)
{
    uint8_t difficulty;
    if(stream_read_byte(stream, &difficulty)) {
        return 1;
    }

    debug("gamemode", "set difficulty to %hhd", difficulty);
    return 0;
}
