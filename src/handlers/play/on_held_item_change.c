#include "net/packet_handler.h"

int on_held_item_change(stream_t *stream)
{
    int8_t slot;
    if(stream_read_byte(stream, &slot)) {
        return 1;
    }

    debug("player", "set selected slot to %hhd", slot);
    return 0;
}
