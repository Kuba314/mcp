#include "packet_handler.h"

#include <stdlib.h>

#include "debug.h"

// packet handlers
static int (*const handshake_packet_handlers[])(unionstream_t *) = { NULL };
static int (*const status_packet_handlers[])(unionstream_t *) = {
    [0x00] = on_server_status_response,
    [0x01] = on_server_pong,
};
static int (*const login_packet_handlers[])(unionstream_t *) = {
    [0x00] = on_login_disconnect,
    [0x01] = on_encryption_request,
    [0x02] = on_login_success,
    [0x03] = on_set_compression,
};
static int (*const play_packet_handlers[])(unionstream_t *) = {
    [0x00] = on_keep_alive,
    [0x01] = on_join_game,
    // [0x02] = on_chat_message,
    // [0x03] = on_time_update,
    // [0x04] = on_entity_equipment,
    [0x05] = on_spawn_position,
    // [0x06] = on_update_health,
    // [0x07] = on_respawn,
    // [0x08] = on_player_position_and_look,
    [0x09] = on_held_item_change,
    // [0x0a] = on_use_bed,
    // [0x0b] = on_animation,
    // [0x0c] = on_spawn_player,
    // [0x0d] = on_collect_item,
    // [0x0e] = on_spawn_object,
    // [0x0f] = on_spawn_mob,
    // [0x10] = on_spawn_painting,
    // [0x11] = on_spawn_experience_orb,
    // [0x12] = on_entity_velocity,
    // [0x13] = on_destroy_entities,
    // [0x14] = on_entity,
    // [0x15] = on_entity_relative_move,
    // [0x16] = on_entity_look,
    // [0x17] = on_entity_look_and_relative_move,
    // [0x18] = on_entity_teleport,
    // [0x19] = on_entity_head_look,
    // [0x1a] = on_entity_status,
    // [0x1b] = on_attach_entity,
    // [0x1c] = on_entity_metadata,
    // [0x1d] = on_entity_effect,
    // [0x1e] = on_remove_entity_effect,
    // [0x1f] = on_set_experience,
    // [0x20] = on_entity_properties,
    // [0x21] = on_chunk_data,
    // [0x22] = on_multi_block_change,
    // [0x23] = on_block_change,
    // [0x24] = on_block_action,
    // [0x25] = on_block_break_animation,
    // [0x26] = on_map_chunk_bulk,
    // [0x27] = on_explosion,
    // [0x28] = on_effect,
    // [0x29] = on_sound_effect,
    // [0x2a] = on_particle,
    // [0x2b] = on_change_game_state,
    // [0x2c] = on_spawn_global_entity,
    // [0x2d] = on_open_window,
    // [0x2e] = on_close_window,
    // [0x2f] = on_set_slot,
    // [0x30] = on_window_items,
    // [0x31] = on_window_property,
    // [0x32] = on_confirm_transaction,
    // [0x33] = on_update_sign,
    // [0x34] = on_map,
    // [0x35] = on_update_block_entity,
    // [0x36] = on_open_sign_editor,
    // [0x37] = on_statistics,
    // [0x38] = on_player_list_item,
    [0x39] = on_player_abilities,
    // [0x3a] = on_tabcomplete,
    // [0x3b] = on_scoreboard_objective,
    // [0x3c] = on_update_score,
    // [0x3d] = on_display_scoreboard,
    // [0x3e] = on_teams,
    [0x3f] = on_plugin_message,
    // [0x40] = on_disconnect,
    [0x41] = on_server_difficulty,
    // [0x42] = on_combat_event,
    // [0x43] = on_camera,
    // [0x44] = on_world_border,
    // [0x45] = on_title,
    // [0x46] = on_set_compression,
    // [0x47] = on_player_list_header_and_footer,
    // [0x48] = on_resource_pack_send,
    // [0x49] = on_update_entity_nbt,
};

static char *get_conn_state_name()
{
    return (g_connection_state == 0)   ? "handshake"
           : (g_connection_state == 1) ? "status"
           : (g_connection_state == 2) ? "login"
           : (g_connection_state == 3) ? "play"
                                       : "yo wtf";
}

conn_state_t g_connection_state = CONN_STATE_HANDSHAKE;

int handle_packet(int32_t packet_id, unionstream_t *stream)
{
    static int (*const *all_packet_handlers[])(unionstream_t *) = {
        handshake_packet_handlers,
        status_packet_handlers,
        login_packet_handlers,
        play_packet_handlers,
    };
    static size_t n_handlers[] = {
        sizeof(handshake_packet_handlers) / sizeof(*handshake_packet_handlers),
        sizeof(status_packet_handlers) / sizeof(*status_packet_handlers),
        sizeof(login_packet_handlers) / sizeof(*login_packet_handlers),
        sizeof(play_packet_handlers) / sizeof(*play_packet_handlers),
    };

    if((uint32_t) packet_id < n_handlers[g_connection_state]) {

        // call packet handler if it exists
        int (*func)(unionstream_t *) = all_packet_handlers[g_connection_state][packet_id];
        if(func != NULL) {
            return func(stream);
        }
    }

    warning_begin("packet_handler", "missing packet handler \"%s\", id 0x%02x, ", get_conn_state_name(), packet_id);
    if(stream->length < 256) {
        warning_frag("dumping: ");
        print_bytes_hex(stream->data + stream->offset, stream->length - stream->offset);
    } else {
        warning_frag("not dumping, too large (%ld)", stream->length - stream->offset);
    }
    warning_end();

    return 0;
}
