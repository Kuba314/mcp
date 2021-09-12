#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t length;
    size_t alloc_length;
} sockbuff_t;

#define SOCKBUFF_INITIAL_SIZE 64

sockbuff_t *sockbuff_create();
int sockbuff_write(sockbuff_t *buff, const void *src, size_t length);
void sockbuff_dumpto(sockbuff_t *buff, int sockfd);

int sockbuff_write_varint(sockbuff_t *buff, int32_t value);
int sockbuff_write_varlong(sockbuff_t *buff, int64_t value);

inline int sockbuff_write_byte(sockbuff_t *buff, uint8_t byte) {
    return sockbuff_write(buff, &byte, 1);
}
inline int sockbuff_write_short(sockbuff_t *buff, uint16_t value) {
    uint8_t bytes[2] = {value >> 8, value & 0xff};
    return sockbuff_write(buff, bytes, 2);
}
inline int sockbuff_write_string(sockbuff_t *buff, const char *src,
                                 size_t length) {
    int ret = sockbuff_write_varint(buff, length);
    return ret | sockbuff_write(buff, src, length);
}
