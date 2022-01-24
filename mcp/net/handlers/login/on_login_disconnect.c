#include "net/stream.h"
#include "utils/dynstring.h"
#include "debug.h"

int on_login_disconnect(stream_t *stream)
{
    string_t *reason = stream_read_string(stream);
    if(reason == NULL) {
        return 1;
    }

    info("login", "disconnected: %.*s", reason->length, reason->s);

    string_free(reason);
    return 0;
}
