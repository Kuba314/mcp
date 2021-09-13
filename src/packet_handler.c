#include "packet_handler.h"

#include <stdlib.h>

#include "debug.h"

// packet handlers
static int (*const handshake_packet_handlers[])(unionstream_t *) = {
    NULL,
};
static int (*const status_packet_handlers[])(unionstream_t *) = {
    on_server_status_response,
    // on_server_pong,
    NULL,
};
static int (*const login_packet_handlers[])(unionstream_t *) = {
    on_login_disconnect,
    on_encryption_request,
    // on_login_success,
    // on_set_compression,
    // on_login_plugin_request,
    NULL,
};
static int (*const play_packet_handlers[])(unionstream_t *) = {
    // on_spawn_entity
    // ...
    NULL,
};

conn_state_t connection_state = CONN_STATE_HANDSHAKE;
int handle_packet(int32_t packet_id, unionstream_t *stream)
{
    static int (*const *all_packet_handlers[])(unionstream_t *) = {
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
        int (*f)(unionstream_t *) =
            all_packet_handlers[connection_state][packet_id];
        if(f != NULL) {
            debug("packet", "id: %d/%d", connection_state, packet_id);
            return f(stream);
        }
    }

    error("packet_handler",
          "No packet handler for state %d and packet id %d, dumping",
          connection_state, packet_id);

    void *tmp = malloc(stream->length - stream->offset);
    if(tmp == NULL) {
        alloc_error();
        return 0;
    }
    stream_read(stream, tmp, stream->length - stream->offset);
    return 0;
}
