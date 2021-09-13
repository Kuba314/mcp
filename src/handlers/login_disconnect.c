#include "unionstream.h"

#include "_string.h"

#include "debug.h"

void on_login_disconnect(unionstream_t *stream) {

    string_t reason;
    stream_read_string(stream, &reason);

    info_begin("login", "disconnected: ");
    print_string(reason);
    info_end();
}
