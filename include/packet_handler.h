#pragma once

#include <stdint.h>

#include "unionstream.h"

typedef enum {
    CONN_STATE_HANDSHAKE,
    CONN_STATE_STATUS,
    CONN_STATE_LOGIN,
    CONN_STATE_PLAY,
} conn_state_t;

extern conn_state_t connection_state;

void handle_packet(int32_t packet_id, unionstream_t *stream);

// ------------------------------------------------------------ packet handlers

// status
void on_server_status_response(unionstream_t *);
void on_server_pong(unionstream_t *);

// login
void on_login_disconnect(unionstream_t *);
void on_encryption_request(unionstream_t *);
void on_login_success(unionstream_t *);
void on_set_compression(unionstream_t *);
void on_login_plugin_request(unionstream_t *);

// play
void on_spawn_entity(unionstream_t *);

