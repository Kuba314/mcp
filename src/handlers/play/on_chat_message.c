#include "packet_handler.h"

int on_chat_message(unionstream_t *stream)
{
    string_t *json_data = stream_read_string(stream);
    if(json_data == NULL) {
        return 1;
    }

    int8_t position;
    if(stream_read_byte(stream, &position)) {
        return 1;
    }

    switch(position) {
    case 0:
        info("chat", "%s", json_data->s);
        break;
    case 1:
        info("sysmsg", "%s", json_data->s);
        break;
    case 2:
        info("hotbmsg", "%s", json_data->s);
        break;
    }

    string_free(json_data);
    return 0;
}
