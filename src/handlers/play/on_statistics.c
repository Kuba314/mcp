#include "net/packet_handler.h"

int on_statistics(stream_t *stream)
{
    int32_t count;
    if(stream_read_varint(stream, &count)) {
        return 1;
    }

    debug("player", "stats: {");
    for(int32_t i = 0; i < count; i++) {
        string_t *field_name = stream_read_string(stream);
        int32_t field_value;
        if(stream_read_varint(stream, &field_value)) {
            return 1;
        }

        debug("player", "   \"%s\": %d,", field_name->s, field_value);

        string_free(field_name);
    }
    debug("player", "}");

    return 0;
}
