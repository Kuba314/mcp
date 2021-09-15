#include "unionstream.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <zlib.h>

#include "debug.h"
#include "varint.h"

int g_compression_threshold = 0;

int stream_read_varint(unionstream_t *stream, int32_t *value)
{
    uint8_t n_read;
    if(read_varint(stream->data + stream->offset, value, &n_read)) {
        return 1;
    }
    stream->offset += n_read;
    return 0;
}

string_t *stream_read_string(unionstream_t *stream)
{
    int32_t length;
    uint8_t n_read;
    if(read_varint(stream->data + stream->offset, &length, &n_read)) {
        return NULL;
    }
    stream->offset += n_read;

    if(length < 0) {
        return NULL;
    }

    string_t *string = string_alloc(length);
    if(string == NULL) {
        alloc_error();
        return NULL;
    }

    if(stream_read(stream, string->s, length)) {
        return NULL;
    }
    return string;
}

void stream_free(unionstream_t *stream)
{
    if(stream->data != NULL) {
        free(stream->data);
    }
    if(stream->en_ctx != NULL) {
        EVP_CIPHER_CTX_free(stream->en_ctx);
    }
    if(stream->de_ctx != NULL) {
        EVP_CIPHER_CTX_free(stream->de_ctx);
    }
}


int stream_read(unionstream_t *stream, void *dst, size_t length)
{
    if(length <= 0) {
        return 0;
    }

    if(length > stream->length - stream->offset) {
        error("stream_read",
              "Requested length (%ld) is larger than data left (%ld)", length,
              stream->length - stream->offset);
        return 1;
    }

    memcpy(dst, stream->data + stream->offset, length);
    stream->offset += length;
    return 0;
}
int stream_write_packet(unionstream_t *stream, const char *buff,
                        size_t length)
{
    // if(length >= stream->compression_threshold) {
    //     // TODO: compress
    // }

    if(stream->is_encrypted) {
        char *encrypted = malloc(length);
        if(!EVP_EncryptUpdate(stream->en_ctx, (uint8_t *) encrypted, NULL, (uint8_t *) buff, length)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }
        buff = encrypted;
    }

    // send packet length
    char varint_bytes[5];
    uint8_t n_bytes = format_varint(varint_bytes, length);

    verbose_begin("sockbuff", "sending \"");
    for(size_t i = 0; i < n_bytes; i++) {
        verbose_frag("\\x%02x", (uint8_t) varint_bytes[i]);
    }
    for(size_t i = 0; i < length; i++) {
        verbose_frag("\\x%02x", (uint8_t) buff[i]);
    }
    verbose_frag("\"");
    verbose_end();

    send(stream->sockfd, varint_bytes, n_bytes, 0);
    send(stream->sockfd, buff, length, 0);
    return 0;
}

/**
 * read packet from stream, decrypt if necessary, inflate if necessary and store
 * into stream cache
 */
int stream_load_packet(unionstream_t *stream)
{

    // I'm so sorry, don't know how to make this better, (maybe create a long
    // varint crypto-enabled function?)
#define N_PRELOADED 5
    // read first 5 bytes (varint max)
    // TODO: FIX THIS SO THAT VARINT IS ABLE TO DECRYPT THE STREAM PLEASEE, CAN'T LOGIN, DOESN'T WORK
    char buff_varint[N_PRELOADED];
    ssize_t _err = recv(stream->sockfd, buff_varint, N_PRELOADED, 0);
    if(_err < 0) {
        perror("stream_load_packet: pre-recv");
        printf("%ld\n", _err);
        return 1;
    } else if(_err != N_PRELOADED) {
        error("load_packet", "disconnected (%ld bytes read)", _err);
        return 1;
    }

    // decode them if necessary
    if(stream->is_encrypted) {
        uint8_t buff_varint_dec[N_PRELOADED];
        int outlen;
        if(!EVP_DecryptUpdate(stream->de_ctx, buff_varint_dec, &outlen,
                              (uint8_t *) buff_varint, N_PRELOADED)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }
        memcpy(buff_varint, buff_varint_dec, N_PRELOADED);
    }

    // read varint packet_length from them
    int32_t packet_length;
    uint8_t n_read;
    if(read_varint(buff_varint, &packet_length, &n_read)) {
        return 1;
    }
    uint8_t overread_bytes = N_PRELOADED - n_read;

    // // check packet length
    // if(packet_length <= 3) {
    //     error("load_packet", "Invalid packet length: %d\n", packet_length);
    //     return 1;
    // }

    char *packet_data = malloc(packet_length);
    if(packet_data == NULL) {
        alloc_error();
        return 1;
    }

    int err = recv(stream->sockfd, packet_data + overread_bytes,
                   packet_length - overread_bytes, MSG_WAITALL);
    if(err < 0) {
        perror("stream_load_packet: recv");
        return 1;
    } else if(err != packet_length - overread_bytes) {
        warning("load_packet", "Didn't read enough, proceeding");
        packet_length = overread_bytes + err;
    }

    if(stream->is_encrypted) {
        char *decrypted = malloc(packet_length);
        if(decrypted == NULL) {
            alloc_error();
            return 1;
        }
        memcpy(decrypted, buff_varint + n_read, overread_bytes);

        int outlen;
        if(!EVP_DecryptUpdate(stream->de_ctx, (uint8_t *) decrypted + overread_bytes,
                              &outlen, (uint8_t *) packet_data + overread_bytes,
                              packet_length - overread_bytes)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }

        void *tmp = packet_data;
        packet_data = decrypted;
        free(tmp);
    } else {
        memcpy(packet_data, buff_varint + n_read, overread_bytes);
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
            error("load_packet", "Invalid decompressed data length: %d",
                  data_length);
            return 1;
        } else if(data_length == 0) {
            error("load_packet", "Data_length is 0, don't know what to do");
            return 1;
        }

        char *decompressed = malloc(data_length);
        if(decompressed == NULL) {
            alloc_error();
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
