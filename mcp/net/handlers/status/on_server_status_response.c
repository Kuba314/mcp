#include "net/packet_handler.h"

int on_server_status_response(stream_t *stream)
{
    string_t *status = stream_read_string(stream);
    if(status == NULL) {
        return 1;
    }

    info("status", "%.*s", status->length, status->s);

    string_free(status);
    return 0;
}
