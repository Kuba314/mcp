#pragma once

#include <stdint.h>

#include "net/stream.h"
#include "utils/buffer.h"
#include "utils/dynstring.h"

// handshake
int send_Handshake(stream_t *stream, int32_t proto_version, uint8_t next_state);

// status
int send_StatusRequest(stream_t *stream);

// login
int send_LoginStart(stream_t *stream, const char *username, size_t username_length);
int send_EncryptionResponse(stream_t *stream, string_t *secret, string_t *token);

// play
int send_KeepAlive(stream_t *stream, int32_t keep_alive_id);
int send_ChatMessage(stream_t *stream, const char *message, size_t message_len);
