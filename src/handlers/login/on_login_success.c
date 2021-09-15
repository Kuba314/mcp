#include "unionstream.h"
#include "_string.h"
#include "debug.h"
#include <string.h>

#include "packet_handler.h"

int on_login_success(unionstream_t *stream)
{
    string_t *uuid = stream_read_string(stream);
    if(uuid == NULL) {
        return 1;
    }

    string_t *username = stream_read_string(stream);
    if(username == NULL) {
        return 1;
    }

    debug("dbg", "%s", username->s);
    debug("dbg", "%s", uuid->s);

    info("login", "success: %s (%s)", username->s, uuid->s);
    g_connection_state = 3;

    string_free(username);
    string_free(uuid);
    return 0;
}
