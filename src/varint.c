#include "varint.h"

#include <stdio.h>
#include <unistd.h>
#include <error.h>

int read_varint(int sockfd, int32_t *result) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte;

    uint32_t uresult = 0;
    do {
        if(bit_offset == 35) {
            fprintf(stderr, "read_varint: VarInt is too big (%d so far)\n", (int32_t) uresult);
            return 1;
        }

        if(read(sockfd, &curr_byte, 1) != 1) {
            perror("read_varint");
            return 1;
        }
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while (curr_byte & 0x80);

    *result = (int32_t) uresult;
    return 0;
}
int write_varint(int sockfd, int32_t value) {
    // cast to unsigned value to make sign bit a part of the value
    uint32_t uvalue = (uint32_t) value;
    uint8_t byte_to_write;
    while(1) {
        if((uvalue & 0xFFFFFF80) == 0) {
            byte_to_write = (uint8_t) uvalue;
            if(write(sockfd, &byte_to_write, 1) != 1) {
                perror("write_varint");
            }
            return 0;
        }

        byte_to_write = (uvalue & 0x7F) | 0x80;
        if(write(sockfd, &byte_to_write, 1) != 1) {
            perror("write_varint");
        }
        // Note: even the sign bit gets shifted
        uvalue >>= 7;
    }

    return 0;
}

int read_varlong(int sockfd, int64_t *result) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte;

    uint64_t uresult = 0;
    do {
        if(bit_offset == 70) {
            fprintf(stderr, "read_varlong: VarLong is too big (%ld so far)\n", (int64_t) uresult);
            return 1;
        }

        if(read(sockfd, &curr_byte, 1) != 1) {
            perror("read_varlong");
        }
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while (curr_byte & 0x80);

    *result = (int64_t) uresult;
    return 0;
}
int write_varlong(int sockfd, int64_t value) {
    // cast to unsigned value to make sign bit a part of the value
    uint64_t uvalue = (uint64_t) value;
    uint8_t byte_to_write;
    while(1) {
        if((uvalue & 0xFFFFFFFFFFFFFF80) == 0) {
            byte_to_write = (uint8_t) uvalue;
            if(write(sockfd, &byte_to_write, 1) != 1) {
                perror("write_varlong");
            }
            return 0;
        }

        byte_to_write = (uvalue & 0x7F) | 0x80;
        if(write(sockfd, &byte_to_write, 1) != 1) {
            perror("write_varlong");
        }
        // Note: even the sign bit gets shifted
        uvalue >>= 7;
    }

    return 0;
}

