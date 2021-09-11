#include "varint.h"
#include "unionstream.h"

#include <stdio.h>
#include <unistd.h>

int read_varint(unionstream_t stream, int32_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint32_t uresult = 0;
    if(n_read != NULL) {
        *n_read = 0;
    }
    do {
        if(bit_offset == 35) {
            fprintf(stderr, "read_varint: VarInt is too big (%d so far)\n", (int32_t) uresult);
            return 1;
        }

        if(stream_read(stream, &curr_byte, 1)) {
            perror("read_varint");
            return 1;
        }
        uresult |= (curr_byte & 0x7f) << bit_offset;
        if(n_read != NULL) {
            (*n_read)++;
        }

        bit_offset += 7;
    } while (curr_byte & 0x80);

    *result = (int32_t) uresult;
    return 0;
}

int read_varlong(unionstream_t stream, int64_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte;

    uint64_t uresult = 0;
    if(n_read != NULL) {
        *n_read = 0;
    }
    do {
        if(bit_offset == 70) {
            fprintf(stderr, "read_varlong: VarLong is too big (%ld so far)\n", (int64_t) uresult);
            return 1;
        }

        if(stream_read(stream, &curr_byte, 1)) {
            perror("read_varlong");
        }
        uresult |= (curr_byte & 0x7f) << bit_offset;
        if(n_read != NULL) {
            (*n_read)++;
        }

        bit_offset += 7;
    } while (curr_byte & 0x80);

    *result = (int64_t) uresult;
    return 0;
}

uint8_t format_varint(uint8_t bytes[5], uint32_t value) {
    uint8_t n_written = 0;
    while(1) {
        n_written++;
        if((value & 0xFFFFFF80) == 0) {
            *bytes++ = (uint8_t) value;
            return n_written;
        }

        *bytes++ = (value & 0x7F) | 0x80;
        value >>= 7;
    }

    return n_written;
}
uint8_t format_varlong(uint8_t bytes[10], uint64_t value) {
    uint8_t n_written = 0;
    while(1) {
        n_written++;
        if((value & 0xFFFFFFFFFFFFFF80) == 0) {
            *bytes++ = (uint8_t) value;
            return n_written;
        }

        *bytes++ = (value & 0x7F) | 0x80;
        value >>= 7;
    }

    return n_written;
}
