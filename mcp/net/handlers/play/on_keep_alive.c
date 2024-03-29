#include "net/packet_handler.h"

#include "net/packets.h"

int on_keep_alive(stream_t *stream)
{
    int32_t keep_alive_id;
    if(stream_read_varint(stream, &keep_alive_id)) {
        return 1;
    }

    send_KeepAlive(stream, keep_alive_id);
    return 0;
}
