#include "packet_handler.h"

int on_spawn_position(unionstream_t *stream)
{
    position_t respawn_point;
    if(stream_read_position(stream, &respawn_point)) {
        return 1;
    }

    debug("player", "%ld set respawn_point to (%d, %d, %d)", sizeof(position_t), respawn_point.x, respawn_point.y, respawn_point.z);
    return 0;
}
