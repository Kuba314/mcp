#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "unionstream.h"

int get_packet_stream(int sockfd, bool compression_enabled,
                      unionstream_t *stream, int32_t *packet_id);

void handle_play_packet(int32_t packet_id, unionstream_t stream);

// packets
int send_Handshake(int sockfd, int32_t proto_version, uint8_t next_state);
int send_StatusRequest(int sockfd);
int send_LoginStart(int sockfd, const char *username);
