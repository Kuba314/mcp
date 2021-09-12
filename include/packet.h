#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "unionstream.h"

int get_packet_stream(int sockfd, bool compression_enabled,
                      unionstream_t *stream, int32_t *packet_id);

void handle_packet(int32_t packet_id, unionstream_t *stream);

// packets
int send_Handshake(int sockfd, int32_t proto_version, uint8_t next_state);
int send_StatusRequest(int sockfd);
int send_LoginStart(int sockfd, const char *username);

// packet handlers
void on_server_status_response(unionstream_t *);
void on_server_pong(unionstream_t *);
void on_login_disconnect(unionstream_t *);
void on_encryption_request(unionstream_t *);
void on_login_success(unionstream_t *);
void on_set_compression(unionstream_t *);
void on_login_plugin_request(unionstream_t *);
void on_spawn_entity(unionstream_t *);

typedef enum {
    CONN_STATE_HANDSHAKE,
    CONN_STATE_STATUS,
    CONN_STATE_LOGIN,
    CONN_STATE_PLAY,
} conn_state_t;

extern conn_state_t connection_state;
