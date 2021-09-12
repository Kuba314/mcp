#include "packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>
#include <sys/socket.h>

#include "varint.h"

static int _get_packet_stream_uncompressed(int sockfd, int32_t packet_length, unionstream_t *stream, int32_t *packet_id)
{
    // read packet id
    uint8_t n_read;
    if(read_varint_fd(sockfd, packet_id, &n_read)) {
        return 1;
    }

    int32_t data_length = packet_length - n_read;
    if(data_length < 0) {
        fprintf(stderr, "_read_packet_uncompressed: Invalid data length: %d\n",
                data_length);
    }

    stream->sockfd = sockfd;
    stream->length = data_length;
    stream->offset = 0;
    return 0;
}

static int _get_packet_stream_compressed(int sockfd, int32_t packet_length, unionstream_t *stream, int32_t *packet_id)
{
    // read uncompressed data length
    int32_t data_length;
    uint8_t data_length_length;
    if(read_varint_fd(sockfd, &data_length, &data_length_length)) {
        return 1;
    }

    // check uncompressed data length
    if(data_length < 0) {
        fprintf(stderr,
                "_read_packet_compressed: Invalid uncompressed data length: %d\n",
                data_length);
        return 1;
    } else if(data_length == 0) {
        fprintf(stderr, "_read_packet_compressed: data_length is 0, don't "
                        "know what to do\n");
        return 1;
    }

    // allocate buffer for compressed data and read to it
    uint32_t compressed_data_length = packet_length - data_length_length;
    stream->data = malloc(compressed_data_length);
    if(stream->data == NULL) {
        perror("_read_packet_compressed: compressed allocation");
        return 1;
    }

    if(recv(sockfd, stream->data, compressed_data_length, MSG_WAITALL) !=
       compressed_data_length) {
        perror("_read_packet_compressed: read");
        return 1;
    }

    // inflate packet data
    stream->length = data_length;
    stream->offset = 0;
    stream->infstream.zalloc = Z_NULL;
    stream->infstream.zfree = Z_NULL;
    stream->infstream.opaque = Z_NULL;

    stream->infstream.avail_in = (uInt) compressed_data_length;
    stream->infstream.next_in = (Bytef *) stream->data;

    inflateInit(&stream->infstream);

    stream_read(stream, (uint8_t *) packet_id, 1);
    return 0;
}

int get_packet_stream(int sockfd, bool compression_enabled, unionstream_t *stream, int32_t *packet_id)
{
    // read packet length
    int32_t packet_length;
    if(read_varint_fd(sockfd, &packet_length, NULL)) {
        return 1;
    }

    // check packet length
    if(packet_length <= 3) {
        fprintf(stderr, "read_packet: Invalid packet length: %d\n",
                packet_length);
        return 1;
    }

    stream->is_compressed = compression_enabled;
    if(compression_enabled) {
        return _get_packet_stream_compressed(sockfd, packet_length, stream, packet_id);
    } else {
        return _get_packet_stream_uncompressed(sockfd, packet_length, stream, packet_id);
    }
}

conn_state_t connection_state = CONN_STATE_HANDSHAKE;
void handle_packet(int32_t packet_id, unionstream_t *stream)
{
    static void (* const handshake_packet_handlers[])(unionstream_t *) = {
        NULL,
    };
    static void (* const status_packet_handlers[])(unionstream_t *) = {
        on_server_status_response,
        // on_server_pong,
        NULL,
    };
    static void (* const login_packet_handlers[])(unionstream_t *) = {
        // on_login_disconnect,
        // on_encryption_request,
        // on_login_success,
        // on_set_compression,
        // on_login_plugin_request,
        NULL,
    };
    static void (* const play_packet_handlers[])(unionstream_t *) = {
        // on_spawn_entity
        // ...
        NULL,
    };

    static void (* const *all_packet_handlers[])(unionstream_t *) = {
        handshake_packet_handlers,
        status_packet_handlers,
        login_packet_handlers,
        play_packet_handlers,
    };
    static size_t n_handlers[] = {
        sizeof(handshake_packet_handlers) / sizeof(*handshake_packet_handlers),
        sizeof(status_packet_handlers) / sizeof(*status_packet_handlers),
        sizeof(login_packet_handlers) / sizeof(*login_packet_handlers),
        sizeof(play_packet_handlers) / sizeof(*play_packet_handlers),
    };


    void (*f)(unionstream_t *);
    if((uint32_t) packet_id >= n_handlers[connection_state] || (f = all_packet_handlers[connection_state][packet_id]) != NULL) {

        fprintf(stderr, "No packet handler for state %d and packet id %d, dumping\n", connection_state, packet_id);

        void *tmp = malloc(stream->length - stream->offset);
        if(tmp == NULL) {
            perror("dumping");
            return;
        }
        stream_read(stream, tmp, stream->length - stream->offset);

        return;
    }

    // call packet handler
    f(stream);
}

void on_server_status_response(unionstream_t *stream) {

    fprintf(stderr, "got server status response:\n\t");
    unsigned char c;
    for(size_t i = 0; i < stream->length; i++) {
        stream_read(stream, &c, 1);
        printf("%c", c);
    }
    printf("\n");
}
