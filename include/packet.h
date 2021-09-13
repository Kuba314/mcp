#pragma once

typedef struct {
    void *data;
} packet_t;

uint8_t *read_packet(int sockfd, bool encryption_enabled, bool compression_enabled) {

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
