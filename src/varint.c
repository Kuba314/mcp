#include "varint.h"

#include <stdio.h>
#include <sys/socket.h>

#include "debug.h"
#include "unionstream.h"

int read_varint(const uint8_t *buff, int32_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint32_t uresult = 0;
    uint8_t n_read_tmp = 0;

    verbose_begin("varint", "reading: ");
    do {
        if(bit_offset == 35) {
            error("read_varint", "VarInt is too big (%d so far)\n", uresult);
            return 1;
        }

        curr_byte = buff[n_read_tmp++];

        verbose_frag("\\x%02x", curr_byte);
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int32_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    verbose_frag(" = %d", *result);
    verbose_end();
    return 0;
}

int read_varlong(const uint8_t *buff, int64_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint64_t uresult = 0;
    uint8_t n_read_tmp = 0;

    verbose_begin("varlong", "reading: ");
    do {
        if(bit_offset == 70) {
            error("read_varint", "VarLong is too big (%ld so far)\n", uresult);
            return 1;
        }

        curr_byte = buff[n_read_tmp++];

        verbose_frag("\\x%02x", curr_byte);
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int64_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    verbose_frag(" = %ld", *result);
    verbose_end();
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

int read_varint_fd(int sockfd, int32_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint32_t uresult = 0;
    uint8_t n_read_tmp = 0;

    verbose_begin("varint_fd", "reading: ");
    do {
        if(bit_offset == 35) {
            error("read_varint_fd", "VarInt is too big (%d so far)\n", uresult);
            return 1;
        }

        if(recv(sockfd, &curr_byte, 1, MSG_WAITALL) != 1) {
            perror("read_varint_fd:recv");
            return 1;
        }
        n_read_tmp++;

        verbose_frag("\\x%02x", curr_byte);
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int32_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    verbose_frag(" = %d", *result);
    verbose_end();
    return 0;
}

int read_varlong_fd(int sockfd, int64_t *result, uint8_t *n_read) {
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint64_t uresult = 0;
    uint8_t n_read_tmp = 0;

    verbose_begin("varlong_fd", "reading: ");
    do {
        if(bit_offset == 70) {
            error("read_varlong_fd", "VarLong is too big (%ld so far)\n", uresult);
            return 1;
        }

        if(recv(sockfd, &curr_byte, 1, MSG_WAITALL) != 1) {
            perror("read_varlong_fd:recv");
            return 1;
        }
        n_read_tmp++;

        verbose_frag("\\x%02x", curr_byte);
        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int32_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    verbose_frag(" = %ld", *result);
    verbose_end();
    return 0;
}
