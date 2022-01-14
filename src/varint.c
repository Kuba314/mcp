#include "varint.h"

#include "stream.h"
#include "debug.h"

int read_varint(const uint8_t *buff, int32_t *result, uint8_t *n_read)
{
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint32_t uresult = 0;
    uint8_t n_read_tmp = 0;

    do {
        if(bit_offset == 35) {
            error("read_varint", "VarInt is too big (%d so far)\n", uresult);
            return 1;
        }

        curr_byte = buff[n_read_tmp++];

        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int32_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    return 0;
}

int read_varlong(const uint8_t *buff, int64_t *result, uint8_t *n_read)
{
    uint8_t bit_offset = 0;
    uint8_t curr_byte = 0;

    uint64_t uresult = 0;
    uint8_t n_read_tmp = 0;

    do {
        if(bit_offset == 70) {
            error("read_varint", "VarLong is too big (%ld so far)\n", uresult);
            return 1;
        }

        curr_byte = buff[n_read_tmp++];

        uresult |= (curr_byte & 0x7f) << bit_offset;

        bit_offset += 7;
    } while(curr_byte & 0x80);

    *result = (int64_t) uresult;
    if(n_read != NULL) {
        *n_read = n_read_tmp;
    }
    return 0;
}

uint8_t format_varint(uint8_t bytes[5], int32_t value)
{
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
uint8_t format_varlong(uint8_t bytes[10], int64_t value)
{
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
