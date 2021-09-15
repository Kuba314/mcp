#include "packet_handler.h"

int on_plugin_message(unionstream_t *stream)
{
    (void) stream;
    verbose("plugin", "skipping a plugin message");
    return 0;
}
