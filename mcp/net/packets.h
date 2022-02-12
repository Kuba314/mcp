#pragma once

#include <stdint.h>

#include "net/stream.h"
#include "utils/buffer.h"
#include "utils/dynstring.h"
#include "net/packet_handler.h"

enum action_id {
    ACTION_ID_RESPAWN = 0,
    ACTION_ID_REQUEST_STATS = 1,
    ACTION_ID_TAKING_INVENTORY = 2,
};

/**
 * v varint
 * V varlong
 * i int
 * h short
 * b byte
 * s c-string
 * S string
 */
int send_packet(stream_t *stream, int packet_id, const char *data_str, ...);

// handshake
inline int send_Handshake(stream_t *stream, int32_t proto_version, const char *ip, uint16_t port, conn_state_t next_state)
{
    g_connection_state = next_state;
    return send_packet(stream, 0x00, "vshb", proto_version, ip, port, next_state);
}

// status
inline int send_StatusRequest(stream_t *stream)
{
    return send_packet(stream, 0x00, "");
}

// login
inline int send_LoginStart(stream_t *stream, const char *username)
{
    return send_packet(stream, 0x00, "s", username);
}
inline int send_EncryptionResponse(stream_t *stream, string_t *enc_aes_key, string_t *enc_verify_token)
{
    return send_packet(stream, 0x01, "SS", enc_aes_key, enc_verify_token);
}

// play
inline int send_KeepAlive(stream_t *stream, int32_t keep_alive_id)
{
    return send_packet(stream, 0x00, "v", keep_alive_id);
}
inline int send_ChatMessage(stream_t *stream, const char *message)
{
    return send_packet(stream, 0x01, "s", message);
}
inline int send_ClientStatus(stream_t *stream, enum action_id action_id)
{
    return send_packet(stream, 0x16, "v", action_id);
}
