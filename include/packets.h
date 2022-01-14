#pragma once

#include <stdint.h>

#include "unionstream.h"
#include "sockbuff.h"
#include "_string.h"

// handshake
int send_Handshake(unionstream_t *stream, int32_t proto_version, uint8_t next_state);

// status
int send_StatusRequest(unionstream_t *stream);

// login
int send_LoginStart(unionstream_t *stream, const char *username, size_t username_length);
int send_EncryptionResponse(unionstream_t *stream, string_t *secret, string_t *token);

// play
int send_KeepAlive(unionstream_t *stream, int32_t keep_alive_id);
int send_ChatMessage(unionstream_t *stream, const char *message, size_t message_len);
