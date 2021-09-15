#include "unionstream.h"
#include "_string.h"
#include "debug.h"

int on_login_disconnect(unionstream_t *stream)
{
    string_t *reason = stream_read_string(stream);
    if(reason == NULL) {
        return 1;
    }

    info_begin("login", "disconnected: ");
    print_string(reason);
    info_end();

    string_free(reason);
    return 0;
}
