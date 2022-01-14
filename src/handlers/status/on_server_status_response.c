#include "packet_handler.h"

int on_server_status_response(stream_t *stream)
{
    string_t *status = stream_read_string(stream);
    if(status == NULL) {
        return 1;
    }

    info_begin("status", "");
    print_string(status);
    info_end();

    string_free(status);
    return 0;
}
