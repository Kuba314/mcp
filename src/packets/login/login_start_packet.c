#include <string.h>

#include "sockbuff.h"
#include "unionstream.h"

int send_LoginStart(unionstream_t *stream, const char *username) {
    size_t name_length = strlen(username);

    sockbuff_t *buff = sockbuff_create();
    if(buff == 0) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_byte(buff, 0);
    err |= sockbuff_write_string(buff, username, name_length);
    if(err) {
        return err;
    }

    stream_write_packet(stream, buff->data, buff->length);
    sockbuff_free(buff);
    return 0;
}
