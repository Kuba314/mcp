#pragma once

#include <stdint.h>
#include <stddef.h>

#include "utils/dynstring.h"

typedef struct {
    char *data;
    size_t length;
    size_t alloc_length;
} buffer_t;

#define SOCKBUFF_INITIAL_SIZE 64

// essential functions
buffer_t *buffer_create();
int buffer_write(buffer_t *buff, const void *src, size_t length);
void buffer_free(buffer_t *buff);

// good-to-have
int buffer_write_varint(buffer_t *buff, int32_t value);
int buffer_write_varlong(buffer_t *buff, int64_t value);

inline int buffer_write_byte(buffer_t *buff, uint8_t byte)
{
    return buffer_write(buff, &byte, 1);
}
inline int buffer_write_short(buffer_t *buff, uint16_t value)
{
    int err = buffer_write_byte(buff, value >> 8);
    return err || buffer_write_byte(buff, value & 0xff);
}
inline int buffer_write_c_string(buffer_t *buff, const char *src, size_t length)
{
    int ret = buffer_write_varint(buff, length);
    return ret || buffer_write(buff, src, length);
}
inline int buffer_write_string(buffer_t *buff, string_t *string)
{
    return buffer_write_c_string(buff, string->s, string->length);
}
