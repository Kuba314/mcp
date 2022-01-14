#include "packets.h"

int send_LoginStart(stream_t *stream, const char *username, size_t username_length)
{
    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        return 1;
    }

    int err = 0;
    err |= buffer_write_byte(buff, 0);
    err |= buffer_write_c_string(buff, username, username_length);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff);
    buffer_free(buff);
    return 0;
}
