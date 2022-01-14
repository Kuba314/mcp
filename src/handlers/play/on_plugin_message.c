#include "packet_handler.h"

int on_plugin_message(stream_t *stream)
{
    (void) stream;
    verbose("plugin", "skipping a plugin message");
    return 0;
}
