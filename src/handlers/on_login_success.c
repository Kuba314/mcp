#include "unionstream.h"
#include "_string.h"
#include "debug.h"

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

    info("login", "success: %s (%s)", username->s, uuid->s);

    string_free(username);
    string_free(uuid);
    return 0;
}
