#include "unionstream.h"

#include <openssl/aes.h>
#include <openssl/err.h>
#include <sys/socket.h>

#include <zlib.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "varint.h"
#include "debug.h"

int stream_read_varint(unionstream_t *stream, int32_t *value) {
    uint8_t n_read;
    if(read_varint(stream->data + stream->offset, value, &n_read)) {
        return 1;
    }
    stream->offset += n_read;
    return 0;
}

int stream_read_string(unionstream_t *stream, string_t *str) {
    int32_t length;
    uint8_t n_read;
    if(read_varint(stream->data + stream->offset, &length, &n_read)) {
        return 1;
    }
    stream->offset += n_read;

    if(length < 0) {
        return 1;
    } else if(length == 0) {
        *str = (string_t){ 0 };
    } else {
        str->s = malloc(length);
        if(str->s == NULL) {
            perror("stream_read_string");
            return 1;
        }
        str->length = length;
        stream_read(stream, str->s, length);
    }
    return 0;
}

void stream_free(unionstream_t *stream) {
    if(stream->data != NULL) {
        free(stream->data);
    }
}

// static int _stream_write_enc(unionstream_t *stream, const uint8_t *buff, size_t length) {

//     uint8_t *enc_buff = malloc(length);
//     if(enc_buff == NULL) {
//         error("alloc", "enc_socket_write");
//         return 1;
//     }

//     int out_length;
//     if(!EVP_EncryptUpdate(stream->en_ctx, enc_buff, &out_length, buff, length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }
//     verbose("enc_socket_enc", "out_length %d", out_length);
//     send(stream->sockfd, enc_buff, length, 0);

//     return 0;
// }

int stream_read(unionstream_t *stream, uint8_t *dst, size_t length) {
    if(length <= 0) {
        return 0;
    }

    if(length > stream->length - stream->offset) {
        fprintf(stderr,
                "stream_read: Requested length is larger than data left\n");
        return 1;
    }

    memcpy(dst, stream->data + stream->offset, length);
    stream->offset += length;
    return 0;
}
int stream_write_packet(unionstream_t *stream, const uint8_t *buff, size_t length) {
    // if(length >= stream->compression_threshold) {
    //     // compress
    // }

    if(stream->is_encrypted) {
        uint8_t *encrypted = malloc(length);
        if(!EVP_EncryptUpdate(stream->en_ctx, encrypted, NULL, buff, length)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }
        buff = encrypted;
    }

    // send packet length
    uint8_t varint_bytes[5];
    uint8_t n_bytes = format_varint(varint_bytes, length);

    debug_begin("sockbuff", "sending \"");
    for(size_t i = 0; i < n_bytes; i++) {
        debug_frag("\\x%02x", varint_bytes[i]);
    }
    for(size_t i = 0; i < length; i++) {
        debug_frag("\\x%02x", buff[i]);
    }
    debug_frag("\"");
    debug_end();

    send(stream->sockfd, varint_bytes, n_bytes, 0);
    send(stream->sockfd, buff, length, 0);
    return 0;
}

/**
 * read packet from stream, decrypt if necessary, inflate if necessary and store
 * into stream cache
 */
int stream_load_packet(unionstream_t *stream) {

    // read first 5 bytes (varint max)
    uint8_t buff_varint[5];
    ssize_t _err = recv(stream->sockfd, buff_varint, 5, 0);
    if(_err != 5) {
        perror("stream_load_packet:recv");
        printf("%ld\n", _err);
        return 1;
    }

    // decode them if necessary
    if(stream->is_encrypted) {
        uint8_t buff_varint_dec[5];
        if(!EVP_DecryptUpdate(stream->de_ctx, buff_varint_dec, NULL, buff_varint, 5)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }
        memcpy(buff_varint, buff_varint_dec, 5);
    }

    // read varint packet_length from them
    int32_t packet_length;
    uint8_t n_read;
    if(read_varint(buff_varint, &packet_length, &n_read)) {
        return 1;
    }
    uint8_t overread_bytes = 5 - n_read;
    debug("dbg", "packet length %d", packet_length);

    // check packet length
    if(packet_length <= 3) {
        error("load_packet", "Invalid packet length: %d\n", packet_length);
        return 1;
    }

    uint8_t *packet_data = malloc(packet_length);
    if(packet_data == NULL) {
        alloc_error(packet_length);
        return 1;
    }
    memcpy(packet_data, buff_varint + n_read, overread_bytes);

    int err = recv(stream->sockfd, packet_data + overread_bytes, packet_length - overread_bytes, MSG_WAITALL);
    if(err < 0) {
        perror("stream_load_packet:recv");
        return 1;
    } else {
        packet_length = err;
    }

    if(stream->is_encrypted) {
        uint8_t *decrypted = malloc(packet_length);
        if(packet_data == NULL) {
            alloc_error(packet_length);
            return 1;
        }

        if(!EVP_DecryptUpdate(stream->de_ctx, decrypted, NULL, packet_data, packet_length)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }

        void *tmp = packet_data;
        packet_data = decrypted;
        free(tmp);
    }

    stream->offset = 0;
    if(!stream->is_compressed) {
        stream->data = packet_data;
        stream->length = packet_length;
    } else {
        // read decompressed data length
        int32_t data_length;
        uint8_t data_length_length;
        if(read_varint(packet_data, &data_length, &data_length_length)) {
            return 1;
        }

        // check decompressed data length
        if(data_length < 0) {
            error("load_packet:", "Invalid decompressed data length: %d", data_length);
            return 1;
        } else if(data_length == 0) {
            error("load_packet", "Data_length is 0, don't know what to do");
            return 1;
        }

        uint8_t *decompressed = malloc(data_length);
        if(decompressed == NULL) {
            alloc_error(data_length);
            return 1;
        }

        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;

        infstream.next_in = (Bytef *) packet_data;
        infstream.avail_in = (uInt) packet_length - data_length_length;
        infstream.next_out = (Bytef *) decompressed;
        infstream.avail_out = (uInt) data_length;

        inflateInit(&infstream);
        inflate(&infstream, Z_FINISH);
        inflateEnd(&infstream);
        free(packet_data);

        stream->data = decompressed;
        stream->length = data_length;
    }
    return 0;
}
