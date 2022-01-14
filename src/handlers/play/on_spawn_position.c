#include "packet_handler.h"

int on_spawn_position(stream_t *stream)
{
    position_t respawn_point;
    if(stream_read_position(stream, &respawn_point)) {
        return 1;
    }

    debug("player", "set respawn_point to (%d, %d, %d)", respawn_point.x, respawn_point.y,
          respawn_point.z);
    return 0;
}
