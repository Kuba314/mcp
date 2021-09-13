#include <stdio.h>

#include "unionstream.h"

void on_server_status_response(unionstream_t *stream) {

    fprintf(stderr, "got server status response:\n\t");
    unsigned char c;
    for(size_t i = 0; i < stream->length; i++) {
        stream_read(stream, &c, 1);
        printf("%c", c);
    }
    printf("\n");
}
