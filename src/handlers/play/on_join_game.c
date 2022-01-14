#include "packet_handler.h"

/*
Entity ID           Int             The player's Entity ID (EID)
Gamemode            Unsigned Byte
    0: Survival, 1: Creative, 2: Adventure, 3: Spectator. Bit 3 (0x8) is the hardcore flag.
Dimension           Byte            -1: Nether, 0: Overworld, 1: End
Difficulty          Unsigned Byte   0: peaceful, 1: easy, 2: normal, 3: hard
Max Players         Unsigned Byte   Used by the client to draw the player list
Level Type          String          default, flat, largeBiomes, amplified, default_1_1
Reduced Debug Info  Boolean         show reduced debug info screen
*/

int on_join_game(stream_t *stream)
{
    int entity_id;
    if(stream_read_int(stream, &entity_id)) {
        return 1;
    }
    uint8_t gamemode;
    if(stream_read_ubyte(stream, &gamemode)) {
        return 1;
    }
    int8_t dimension;
    if(stream_read_byte(stream, &dimension)) {
        return 1;
    }
    uint8_t difficulty;
    if(stream_read_ubyte(stream, &difficulty)) {
        return 1;
    }
    uint8_t max_players;
    if(stream_read_ubyte(stream, &max_players)) {
        return 1;
    }
    string_t *level_type = stream_read_string(stream);
    if(level_type == NULL) {
        return 1;
    }
    bool reduced_debug_info;
    if(stream_read_bool(stream, &reduced_debug_info)) {
        return 1;
    }

    debug("join_game", "eid(0x%x) gm(%u) dim(%d) diff(%u) maxpl(%u) lvlt(%s) redudeceddbgi(%u)",
          entity_id, gamemode, dimension, difficulty, max_players, level_type->s,
          reduced_debug_info);
    string_free(level_type);
    return 0;
}
