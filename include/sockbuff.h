#pragma once

#include <stdint.h>
#include <stddef.h>

#include "_string.h"

typedef struct {
    char *data;
    size_t length;
    size_t alloc_length;
} sockbuff_t;

#define SOCKBUFF_INITIAL_SIZE 64

// essential functions
sockbuff_t *sockbuff_create();
int sockbuff_write(sockbuff_t *buff, const void *src, size_t length);
void sockbuff_free(sockbuff_t *buff);

// good-to-have
int sockbuff_write_varint(sockbuff_t *buff, int32_t value);
int sockbuff_write_varlong(sockbuff_t *buff, int64_t value);

inline int sockbuff_write_byte(sockbuff_t *buff, uint8_t byte)
{
    return sockbuff_write(buff, &byte, 1);
}
inline int sockbuff_write_short(sockbuff_t *buff, uint16_t value)
{
    int err = sockbuff_write_byte(buff, value >> 8);
    return err || sockbuff_write_byte(buff, value & 0xff);
}
inline int sockbuff_write_c_string(sockbuff_t *buff, const char *src,
                                 size_t length)
{
    int ret = sockbuff_write_varint(buff, length);
    return ret || sockbuff_write(buff, src, length);
}
inline int sockbuff_write_string(sockbuff_t *buff, string_t *string)
{
    return sockbuff_write_c_string(buff, string->s, string->length);
}
