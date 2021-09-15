#include "packet_handler.h"

int on_player_abilities(unionstream_t *stream)
{
    uint8_t flags;
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

    debug_begin("player", "abilities: ");
    debug_frag("invuln(%hhd) ", flags & 1);
    debug_frag("fly(%hhd) ", (flags >> 1) & 1);
    debug_frag("allow_fly(%hhd) ", (flags >> 2) & 1);
    debug_frag("creative(%hhd) ", (flags >> 3) & 1);
    debug_frag("fly_speed(%.02f) ", flying_speed);
    debug_frag("fov_mod(%.02f)", fov_mod);
    debug_end();
    return 0;
}
