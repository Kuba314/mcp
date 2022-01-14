#include "packet_handler.h"

int on_server_difficulty(stream_t *stream)
{
    uint8_t difficulty;
    if(stream_read_ubyte(stream, &difficulty)) {
        return 1;
    }

    debug("gamemode", "set difficulty to %hhd", difficulty);
    return 0;
}
