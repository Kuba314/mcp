#pragma once

#include <stdint.h>

#include "unionstream.h"
#include "_string.h"
#include "debug.h"

typedef enum
{
    CONN_STATE_HANDSHAKE,
    CONN_STATE_STATUS,
    CONN_STATE_LOGIN,
    CONN_STATE_PLAY,
} conn_state_t;

extern conn_state_t g_connection_state;

int handle_packet(int32_t packet_id, unionstream_t *stream);

// ------------------------------------------------------------ packet handlers

// status
int on_server_status_response(unionstream_t *);
int on_server_pong(unionstream_t *);

// login
int on_login_disconnect(unionstream_t *);
int on_encryption_request(unionstream_t *);
int on_login_success(unionstream_t *);
int on_set_compression(unionstream_t *);

// play
int on_keep_alive(unionstream_t *);
int on_join_game(unionstream_t *);
int on_chat_message(unionstream_t *);
int on_time_update(unionstream_t *);
int on_entity_equipment(unionstream_t *);
int on_spawn_position(unionstream_t *);
int on_update_health(unionstream_t *);
int on_respawn(unionstream_t *);
int on_player_position_and_look(unionstream_t *);
int on_held_item_change(unionstream_t *);
int on_use_bed(unionstream_t *);
int on_animation(unionstream_t *);
int on_spawn_player(unionstream_t *);
int on_collect_item(unionstream_t *);
int on_spawn_object(unionstream_t *);
int on_spawn_mob(unionstream_t *);
int on_spawn_painting(unionstream_t *);
int on_spawn_experience_orb(unionstream_t *);
int on_entity_velocity(unionstream_t *);
int on_destroy_entities(unionstream_t *);
int on_entity(unionstream_t *);
int on_entity_relative_move(unionstream_t *);
int on_entity_look(unionstream_t *);
int on_entity_look_and_relative_move(unionstream_t *);
int on_entity_teleport(unionstream_t *);
int on_entity_head_look(unionstream_t *);
int on_entity_status(unionstream_t *);
int on_attach_entity(unionstream_t *);
int on_entity_metadata(unionstream_t *);
int on_entity_effect(unionstream_t *);
int on_remove_entity_effect(unionstream_t *);
int on_set_experience(unionstream_t *);
int on_entity_properties(unionstream_t *);
int on_chunk_data(unionstream_t *);
int on_multi_block_change(unionstream_t *);
int on_block_change(unionstream_t *);
int on_block_action(unionstream_t *);
int on_block_break_animation(unionstream_t *);
int on_map_chunk_bulk(unionstream_t *);
int on_explosion(unionstream_t *);
int on_effect(unionstream_t *);
int on_sound_effect(unionstream_t *);
int on_particle(unionstream_t *);
int on_change_game_state(unionstream_t *);
int on_spawn_global_entity(unionstream_t *);
int on_open_window(unionstream_t *);
int on_close_window(unionstream_t *);
int on_set_slot(unionstream_t *);
int on_window_items(unionstream_t *);
int on_window_property(unionstream_t *);
int on_confirm_transaction(unionstream_t *);
int on_update_sign(unionstream_t *);
int on_map(unionstream_t *);
int on_update_block_entity(unionstream_t *);
int on_open_sign_editor(unionstream_t *);
int on_statistics(unionstream_t *);
int on_player_list_item(unionstream_t *);
int on_player_abilities(unionstream_t *);
int on_tabcomplete(unionstream_t *);
int on_scoreboard_objective(unionstream_t *);
int on_update_score(unionstream_t *);
int on_display_scoreboard(unionstream_t *);
int on_teams(unionstream_t *);
int on_plugin_message(unionstream_t *);
int on_disconnect(unionstream_t *);
int on_server_difficulty(unionstream_t *);
int on_combat_event(unionstream_t *);
int on_camera(unionstream_t *);
int on_world_border(unionstream_t *);
int on_title(unionstream_t *);
int on_set_compression(unionstream_t *);
int on_player_list_header_and_footer(unionstream_t *);
int on_resource_pack_send(unionstream_t *);
int on_update_entity_nbt(unionstream_t *);

// int on_spawn_entity(unionstream_t *);
