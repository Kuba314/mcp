#pragma once

#include <stdint.h>

#include "unionstream.h"

// handshake
int send_Handshake(unionstream_t *stream, int32_t proto_version,
                   uint8_t next_state);

// status
int send_StatusRequest(unionstream_t *stream);

// login
int send_LoginStart(unionstream_t *stream, const char *username, size_t username_length);
int send_EncryptionResponse(unionstream_t *stream, size_t secret_length,
                            const char *secret, size_t token_length,
                            const char *token);
