#include "net/stream.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <zlib.h>

#include "data/varint.h"
#include "debug.h"

size_t g_compression_threshold = 0;

int stream_create(int sockfd, stream_t **stream)
{
    *stream = calloc(1, sizeof(stream_t));
    if(*stream == NULL) {
        alloc_error();
        return 1;
    }

    if(sem_init(&(*stream)->lock, 0, 1)) {
        perror("sem_init");
        free(stream);
        return 1;
    }

    (*stream)->sockfd = sockfd;
    return 0;
}
void stream_free_data(stream_t *stream)
{
    free(stream->data);
    stream->data = NULL;
    stream->length = 0;
    stream->offset = 0;
}
void stream_free(stream_t *stream)
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
    free(stream);
}

/**
 * read packet from stream, decrypt if necessary, inflate if necessary and store
 * into stream cache
 */
int stream_load_packet(stream_t *stream)
{
    int32_t packet_length;
    if(stream_read_varint_directly(stream, &packet_length)) {
        return 1;
    }

    // check packet length
    if(packet_length < 0) {
        error("load_packet", "Invalid packet length: %d\n", packet_length);
        return 1;
    }

    uint8_t *packet_data = malloc(packet_length);
    if(packet_data == NULL) {
        alloc_error();
        return 1;
    }

    int err = recv(stream->sockfd, packet_data, packet_length, MSG_WAITALL);
    if(err < 0) {
        perror("stream_load_packet: recv");
        return 1;
    } else if(err != packet_length) {
        warn("load_packet", "Didn't read enough, proceeding");
        packet_length = err;
    }

    if(stream->is_encrypted) {
        uint8_t *decrypted = malloc(packet_length);
        if(decrypted == NULL) {
            alloc_error();
            return 1;
        }

        int outlen;
        if(!EVP_DecryptUpdate(stream->de_ctx, decrypted, &outlen, packet_data, packet_length)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }

        uint8_t *tmp = packet_data;
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
            error("load_packet", "Invalid decompressed data length: %d", data_length);
            return 1;
        } else if(data_length == 0) {
            // this packet is not compressed
            stream->data = packet_data;
            stream->length = packet_length;
            stream->offset = data_length_length;
            return 0;
        }

        uint8_t *decompressed = malloc(data_length);
        if(decompressed == NULL) {
            alloc_error();
            return 1;
        }

        z_stream infstream;
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        infstream.opaque = Z_NULL;

        infstream.next_in = (Bytef *) packet_data + data_length_length;
        infstream.avail_in = (uInt) packet_length - data_length_length;
        infstream.next_out = (Bytef *) decompressed;
        infstream.avail_out = (uInt) data_length;

        inflateInit(&infstream);
        int inf_err = inflate(&infstream, Z_FINISH);
        if(inf_err < 0) {
            error("zlib", "couldn't inflate (error %d)\n", inf_err);
            return 1;
        }
        inflateEnd(&infstream);
        free(packet_data);

        stream->data = decompressed;
        stream->length = data_length;
    }
    return 0;
}
int stream_write_packet(stream_t *stream, buffer_t *buff)
{
    sem_wait(&stream->lock);

    verbose_begin("stream", "sending \"");
    verbose_bytes(buff, buff->length);
    verbose_frag("\"");
    verbose_end();

    // copy data to own buffer
    void *src = malloc(buff->length);
    if(src == NULL) {
        alloc_error();
        sem_post(&stream->lock);
        return 1;
    }

    uint8_t *after_compression;
    size_t after_compression_length;

    // will have to prepend decompressed data size
    if(stream->is_compressed) {
        size_t compressed_length;
        uint8_t data_len_varint[5];
        uint8_t data_len_nbytes;

        // only compress if length is over threshold
        if(buff->length >= g_compression_threshold) {
            z_stream defstream;
            defstream.zalloc = Z_NULL;
            defstream.zfree = Z_NULL;
            defstream.opaque = Z_NULL;

            defstream.next_in = (Bytef *) buff->data;
            defstream.avail_in = (uInt) buff->length;
            defstream.next_out = (Bytef *) src;
            defstream.avail_out = (uInt) buff->length;

            deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
            deflate(&defstream, Z_FINISH);
            deflateEnd(&defstream);

            compressed_length = buff->length - defstream.avail_out;
            data_len_nbytes = format_varint(data_len_varint, buff->length);
        } else {
            compressed_length = buff->length;
            data_len_nbytes = format_varint(data_len_varint, 0);
            memcpy(src, buff->data, buff->length);
        }

        uint8_t pkt_len_varint[5];
        uint8_t pkt_len_nbytes = format_varint(pkt_len_varint, compressed_length + data_len_nbytes);

        // memcpy everything neatly into a new buffer
        after_compression_length = pkt_len_nbytes + data_len_nbytes + compressed_length;
        after_compression = malloc(after_compression_length);
        if(after_compression == NULL) {
            alloc_error();
            sem_post(&stream->lock);
            return 1;
        }
        memcpy(after_compression, pkt_len_varint, pkt_len_nbytes);
        memcpy(after_compression + pkt_len_nbytes, data_len_varint, data_len_nbytes);
        memcpy(after_compression + pkt_len_nbytes + data_len_nbytes, src, compressed_length);
        free(src);
    } else {
        uint8_t pkt_len_varint[5];
        uint8_t pkt_len_nbytes = format_varint(pkt_len_varint, buff->length);

        after_compression_length = pkt_len_nbytes + buff->length;
        after_compression = malloc(after_compression_length);
        if(after_compression == NULL) {
            alloc_error();
            sem_post(&stream->lock);
            return 1;
        }
        memcpy(after_compression, pkt_len_varint, pkt_len_nbytes);
        memcpy(after_compression + pkt_len_nbytes, buff->data, buff->length);
        free(src);
    }

    uint8_t *after_encryption = after_compression;
    size_t after_encryption_length = after_compression_length;

    // pass data through AES if the stream is encrypted
    if(stream->is_encrypted) {
        after_encryption = malloc(after_compression_length);
        if(after_encryption == NULL) {
            alloc_error();
            sem_post(&stream->lock);
            return 1;
        }

        int outlen;
        if(!EVP_EncryptUpdate(stream->en_ctx, after_encryption, &outlen, after_compression,
                              after_compression_length)) {
            ERR_print_errors_fp(stderr);
            sem_post(&stream->lock);
            return 1;
        }
        free(after_compression);
    }

    send(stream->sockfd, after_encryption, after_encryption_length, 0);
    free(after_encryption);
    sem_post(&stream->lock);
    return 0;
}

// read directly from sockfd, decrypt if encrypted
int stream_read_directly(stream_t *stream, void *dst, size_t length)
{
    ssize_t n_read;
    if(stream->is_encrypted) {
        void *encrypted = malloc(length);
        if(encrypted == NULL) {
            alloc_error();
            return 1;
        }

        if((n_read = recv(stream->sockfd, encrypted, length, MSG_WAITALL)) < 0) {
            perror("stream_read_directly: recv");
            return 1;
        } else if(n_read != (ssize_t) length) {
            error("stream_read_directly", "couldn't read enough (%ld / %ld)", n_read, length);
            return 1;
        }

        int outlen;
        if(!EVP_DecryptUpdate(stream->de_ctx, dst, &outlen, encrypted, length)) {
            ERR_print_errors_fp(stderr);
            return 1;
        }
        free(encrypted);
    } else {
        if((n_read = recv(stream->sockfd, dst, length, MSG_WAITALL)) < 0) {
            perror("stream_read_directly: recv");
            return 1;
        } else if(n_read != (ssize_t) length) {
            error("stream_read_directly", "couldn't read enough (%ld / %ld)", n_read, length);
            return 1;
        }
    }
    return 0;
}
int stream_read_varint_directly(stream_t *stream, int32_t *value)
{
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint32_t uresult = 0;

    do {
        if(bit_offset == 35) {
            error("varint_direct", "VarInt is too big (%d so far)\n", uresult);
            return 1;
        }

        if(stream_read_directly(stream, &curr_byte, 1)) {
            return 1;
        }

        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *value = (int32_t) uresult;
    return 0;
}

// read from stream buffer to dst
int stream_read(stream_t *stream, void *dst, size_t length)
{
    if(length <= 0) {
        return 0;
    }

    if(length > stream->length - stream->offset) {
        error("stream_read", "Requested length (%ld) is larger than data left (%ld)", length,
              stream->length - stream->offset);
        return 1;
    }

    memcpy(dst, stream->data + stream->offset, length);
    stream->offset += length;
    return 0;
}
// same as stream_read, but stores bytes in reverse order
int stream_read_rev(stream_t *stream, void *dst, size_t length)
{
    if(length <= 0) {
        return 0;
    }

    if(length > stream->length - stream->offset) {
        error("stream_read", "Requested length (%ld) is larger than data left (%ld)", length,
              stream->length - stream->offset);
        return 1;
    }

    // memcpy, but reversed
    for(size_t i = 0; i < length; i++) {
        ((uint8_t *) dst)[length - i - 1] = stream->data[stream->offset + i];
    }
    stream->offset += length;
    return 0;
}

extern int stream_read_bool(stream_t *stream, bool *value);
extern int stream_read_byte(stream_t *stream, int8_t *value);
extern int stream_read_short(stream_t *stream, int16_t *value);
extern int stream_read_int(stream_t *stream, int32_t *value);
extern int stream_read_long(stream_t *stream, int64_t *value);

extern int stream_read_ubyte(stream_t *stream, uint8_t *value);
extern int stream_read_ushort(stream_t *stream, uint16_t *value);
extern int stream_read_float(stream_t *stream, float *value);
extern int stream_read_double(stream_t *stream, double *value);

string_t *stream_read_string(stream_t *stream)
{
    int32_t length;
    if(stream_read_varint(stream, &length)) {
        return NULL;
    }

    if(length < 0) {
        error("stream_read_string", "string with negative length (%d)", length);
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
    string->s[length] = '\0';
    return string;
}

int stream_read_varint(stream_t *stream, int32_t *value)
{
    uint8_t n_read;
    if(read_varint(stream->data + stream->offset, value, &n_read)) {
        return 1;
    }
    stream->offset += n_read;
    return 0;
}
int stream_read_position(stream_t *stream, position_t *position)
{
    uint64_t tmp;
    if(stream_read_rev(stream, &tmp, 8)) {
        return 1;
    }
    position->x = (tmp >> 38);
    position->y = (tmp >> 26) & 0xfff;
    position->z = (tmp & 0x3ffffff);
    position->x = (position->x >= (1 << 25)) ? position->x - (1 << 26) : position->x;
    position->y = (position->y >= (1 << 11)) ? position->y - (1 << 12) : position->y;
    position->z = (position->z >= (1 << 25)) ? position->z - (1 << 26) : position->z;
    return 0;
}
