#include "packet_handler.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

// packet handlers
static void (*const handshake_packet_handlers[])(unionstream_t *) = {
    NULL,
};
static void (*const status_packet_handlers[])(unionstream_t *) = {
    on_server_status_response,
    // on_server_pong,
    NULL,
};
static void (*const login_packet_handlers[])(unionstream_t *) = {
    on_login_disconnect,
    on_encryption_request,
    // on_login_success,
    // on_set_compression,
    // on_login_plugin_request,
    NULL,
};
static void (*const play_packet_handlers[])(unionstream_t *) = {
    // on_spawn_entity
    // ...
    NULL,
};

conn_state_t connection_state = CONN_STATE_HANDSHAKE;
void handle_packet(int32_t packet_id, unionstream_t *stream) {
    static void (*const *all_packet_handlers[])(unionstream_t *) = {
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

    if((uint32_t) packet_id < n_handlers[connection_state]) {

        // call packet handler ptr if not NULL
        void (*f)(unionstream_t *) =
            all_packet_handlers[connection_state][packet_id];
        if(f != NULL) {
            debug("packet", "id: %d/%d", connection_state, packet_id);
            f(stream);
            return;
        }
    }

    error("packet_handler", "No packet handler for state %d and packet id %d, dumping", connection_state, packet_id);

    void *tmp = malloc(stream->length - stream->offset);
    if(tmp == NULL) {
        perror("dumping");
        return;
    }
    stream_read(stream, tmp, stream->length - stream->offset);
}
