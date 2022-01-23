#pragma once

#include <stdint.h>

#include "net/stream.h"
#include "utils/dynstring.h"
#include "debug.h"

typedef enum
{
    CONN_STATE_HANDSHAKE,
    CONN_STATE_STATUS,
    CONN_STATE_LOGIN,
    CONN_STATE_PLAY,
} conn_state_t;

extern conn_state_t g_connection_state;

int handle_packet(int32_t packet_id, stream_t *stream);

// ------------------------------------------------------------ packet handlers

// status
int on_server_status_response(stream_t *);
int on_server_pong(stream_t *);

// login
int on_login_disconnect(stream_t *);
int on_encryption_request(stream_t *);
int on_login_success(stream_t *);
int on_set_compression(stream_t *);

// play
int on_keep_alive(stream_t *);
int on_join_game(stream_t *);
int on_chat_message(stream_t *);
int on_time_update(stream_t *);
int on_entity_equipment(stream_t *);
int on_spawn_position(stream_t *);
int on_update_health(stream_t *);
int on_respawn(stream_t *);
int on_player_position_and_look(stream_t *);
int on_held_item_change(stream_t *);
int on_use_bed(stream_t *);
int on_animation(stream_t *);
int on_spawn_player(stream_t *);
int on_collect_item(stream_t *);
int on_spawn_object(stream_t *);
int on_spawn_mob(stream_t *);
int on_spawn_painting(stream_t *);
int on_spawn_experience_orb(stream_t *);
int on_entity_velocity(stream_t *);
int on_destroy_entities(stream_t *);
int on_entity(stream_t *);
int on_entity_relative_move(stream_t *);
int on_entity_look(stream_t *);
int on_entity_look_and_relative_move(stream_t *);
int on_entity_teleport(stream_t *);
int on_entity_head_look(stream_t *);
int on_entity_status(stream_t *);
int on_attach_entity(stream_t *);
int on_entity_metadata(stream_t *);
int on_entity_effect(stream_t *);
int on_remove_entity_effect(stream_t *);
int on_set_experience(stream_t *);
int on_entity_properties(stream_t *);
int on_chunk_data(stream_t *);
int on_multi_block_change(stream_t *);
int on_block_change(stream_t *);
int on_block_action(stream_t *);
int on_block_break_animation(stream_t *);
int on_map_chunk_bulk(stream_t *);
int on_explosion(stream_t *);
int on_effect(stream_t *);
int on_sound_effect(stream_t *);
int on_particle(stream_t *);
int on_change_game_state(stream_t *);
int on_spawn_global_entity(stream_t *);
int on_open_window(stream_t *);
int on_close_window(stream_t *);
int on_set_slot(stream_t *);
int on_window_items(stream_t *);
int on_window_property(stream_t *);
int on_confirm_transaction(stream_t *);
int on_update_sign(stream_t *);
int on_map(stream_t *);
int on_update_block_entity(stream_t *);
int on_open_sign_editor(stream_t *);
int on_statistics(stream_t *);
int on_player_list_item(stream_t *);
int on_player_abilities(stream_t *);
int on_tabcomplete(stream_t *);
int on_scoreboard_objective(stream_t *);
int on_update_score(stream_t *);
int on_display_scoreboard(stream_t *);
int on_teams(stream_t *);
int on_plugin_message(stream_t *);
int on_disconnect(stream_t *);
int on_server_difficulty(stream_t *);
int on_combat_event(stream_t *);
int on_camera(stream_t *);
int on_world_border(stream_t *);
int on_title(stream_t *);
int on_set_compression(stream_t *);
int on_player_list_header_and_footer(stream_t *);
int on_resource_pack_send(stream_t *);
int on_update_entity_nbt(stream_t *);

// int on_spawn_entity(stream_t *);
