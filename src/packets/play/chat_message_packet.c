#include "packets.h"

int send_ChatMessage(stream_t *stream, const char *message, size_t message_len)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_byte(buff, 1);
    err |= buffer_write_c_string(buff, message, message_len);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
