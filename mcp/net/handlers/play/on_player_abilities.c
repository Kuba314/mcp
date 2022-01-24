#include "net/packet_handler.h"

int on_player_abilities(stream_t *stream)
{
    int8_t flags;
    if(stream_read_byte(stream, &flags)) {
        return 1;
    }

    float flying_speed;
    if(stream_read_float(stream, &flying_speed)) {
        return 1;
    }
    float fov_mod;
    if(stream_read_float(stream, &fov_mod)) {
        return 1;
    }

    debug("player",
          "abilities: invuln(%hhd) fly(%hhd) allow_fly(%hhd) creative(%hhd) fly_speed(%.02f) "
          "fov_mod(%.02f)",
          flags & 1, (flags >> 1) & 1, (flags >> 2) & 1, (flags >> 3) & 1, flying_speed, fov_mod);
    return 0;
}
