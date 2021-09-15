#include "packet_handler.h"

#include <stdlib.h>

#include "debug.h"

// packet handlers
static int (*const handshake_packet_handlers[])(unionstream_t *) = { NULL };
static int (*const status_packet_handlers[])(unionstream_t *) = {
    on_server_status_response, on_server_pong
};
static int (*const login_packet_handlers[])(unionstream_t *) = {
    on_login_disconnect, on_encryption_request, on_login_success,
    on_set_compression
};
static int (*const play_packet_handlers[])(unionstream_t *) = {
    on_keep_alive,
    on_join_game,
    // on_chat_message,
    // on_time_update,
    // on_entity_equipment,
    [0x05] = on_spawn_position,
    // on_update_health,
    // on_respawn,
    // on_player_position_and_look,
    [0x09] = on_held_item_change,
    // on_use_bed,
    // on_animation,
    // on_spawn_player,
    // on_collect_item,
    // on_spawn_object,
    // on_spawn_mob,
    // on_spawn_painting,
    // on_spawn_experience_orb,
    // on_entity_velocity,
    // on_destroy_entities,
    // on_entity,
    // on_entity_relative_move,
    // on_entity_look,
    // on_entity_look_and_relative_move,
    // on_entity_teleport,
    // on_entity_head_look,
    // on_entity_status,
    // on_attach_entity,
    // on_entity_metadata,
    // on_entity_effect,
    // on_remove_entity_effect,
    // on_set_experience,
    // on_entity_properties,
    // on_chunk_data,
    // on_multi_block_change,
    // on_block_change,
    // on_block_action,
    // on_block_break_animation,
    // on_map_chunk_bulk,
    // on_explosion,
    // on_effect,
    // on_sound_effect,
    // on_particle,
    // on_change_game_state,
    // on_spawn_global_entity,
    // on_open_window,
    // on_close_window,
    // on_set_slot,
    // on_window_items,
    // on_window_property,
    // on_confirm_transaction,
    // on_update_sign,
    // on_map,
    // on_update_block_entity,
    // on_open_sign_editor,
    // on_statistics,
    // on_player_list_item,
    [0x39] = on_player_abilities,
    // on_tabcomplete,
    // on_scoreboard_objective,
    // on_update_score,
    // on_display_scoreboard,
    // on_teams,
    [0x3f] = on_plugin_message,
    // on_disconnect,
    [0x41] = on_server_difficulty,
    // on_combat_event,
    // on_camera,
    // on_world_border,
    // on_title,
    // on_set_compression,
    // on_player_list_header_and_footer,
    // on_resource_pack_send,
    // on_update_entity_nbt,
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

        // call packet handler ptr if not NULL
        int (*f)(unionstream_t *) =
            all_packet_handlers[g_connection_state][packet_id];
        if(f != NULL) {

            // debug("packet", "[%s] id: %d", get_conn_state_name(), packet_id);
            return f(stream);
        }
    }

    error("packet_handler",
          "No packet handler for state \"%s\" and packet id 0x%02x, dumping",
          get_conn_state_name(), packet_id);

    verbose_begin("dump", "");
    print_string_c_hex(stream->data + stream->offset, stream->length - stream->offset);
    verbose_end();

    // void *tmp = malloc(stream->length - stream->offset);
    // if(tmp == NULL) {
    //     alloc_error();
    //     return 0;
    // }
    // stream_read(stream, tmp, stream->length - stream->offset);
    // free(tmp);
    return 0;
}
