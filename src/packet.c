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
    if(read_varint(SOCK2STREAM(sockfd), packet_id, &n_read)) {
        return 1;
    }

    int32_t data_length = packet_length - n_read;
    if(data_length < 0) {
        fprintf(stderr, "_read_packet_uncompressed: Invalid data length: %d\n",
                data_length);
    }

    stream->sockfd = sockfd;
    stream->length = data_length;
    return 0;
}

static int _get_packet_stream_compressed(int sockfd, int32_t packet_length, unionstream_t *stream, int32_t *packet_id)
{
    // read uncompressed data length
    int32_t data_length;
    uint8_t data_length_length;
    if(read_varint(SOCK2STREAM(sockfd), &data_length, &data_length_length)) {
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
    stream->infstream.zalloc = Z_NULL;
    stream->infstream.zfree = Z_NULL;
    stream->infstream.opaque = Z_NULL;

    stream->infstream.avail_in = (uInt) compressed_data_length;
    stream->infstream.next_in = (Bytef *) stream->data;

    inflateInit(&stream->infstream);

    stream_read(*stream, packet_id, 1);
    return 0;
}

int get_packet_stream(int sockfd, bool compression_enabled, unionstream_t *stream, int32_t *packet_id)
{
    // read packet length
    int32_t packet_length;
    if(read_varint(SOCK2STREAM(sockfd), &packet_length, NULL)) {
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

void handle_play_packet(int32_t packet_id, unionstream_t stream)
{
    void (* packet_handlers[])(unionstream_t) = {
        NULL
    };

    // call packet handler
    packet_handlers[packet_id](stream);
}
