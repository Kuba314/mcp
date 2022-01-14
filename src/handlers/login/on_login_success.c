#include "stream.h"
#include "dynstring.h"
#include "debug.h"
#include <string.h>

#include "packet_handler.h"

int on_login_success(stream_t *stream)
{
    string_t *uuid = stream_read_string(stream);
    if(uuid == NULL) {
        return 1;
    }

    string_t *username = stream_read_string(stream);
    if(username == NULL) {
        return 1;
    }

    info("login", "Successfully logged in as: %s (%s)", username->s, uuid->s);
    g_connection_state = 3;

    string_free(username);
    string_free(uuid);
    return 0;
}
