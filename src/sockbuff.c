#include "sockbuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "varint.h"
#include "debug.h"

void sockbuff_free(sockbuff_t *buff) {
    if(buff->data != NULL) {
        free(buff->data);
    }
    free(buff);
}

sockbuff_t *sockbuff_create() {
    sockbuff_t *buff = malloc(sizeof(sockbuff_t));
    if(buff == NULL) {
        perror("sockbuff_create");
        return NULL;
    }

    buff->data = malloc(SOCKBUFF_INITIAL_SIZE);
    if(buff->data == NULL) {
        perror("sockbuff_create");
        free(buff);
        return NULL;
    }

    buff->length = 0;
    buff->alloc_length = SOCKBUFF_INITIAL_SIZE;
    return buff;
}

int sockbuff_write(sockbuff_t *buff, const void *src, size_t length) {
    size_t new_length = buff->length + length;
    while(new_length > buff->alloc_length) {
        buff->alloc_length *= 2;
        void *tmp = realloc(buff->data, buff->alloc_length);
        if(tmp == NULL) {
            perror("sockbuff_write");
            sockbuff_free(buff);
            return 1;
        }

        buff->data = tmp;
    }

    memcpy(buff->data + buff->length, src, length);
    buff->length = new_length;
    return 0;
}

extern int sockbuff_write_byte(sockbuff_t *buff, uint8_t byte);
extern int sockbuff_write_short(sockbuff_t *buff, uint16_t value);
extern int sockbuff_write_string(sockbuff_t *buff, const char *src,
                                 size_t length);
int sockbuff_write_varint(sockbuff_t *buff, int32_t value) {
    uint8_t bytes[5];
    size_t n_bytes = format_varint(bytes, value);
    return sockbuff_write(buff, bytes, n_bytes);
}
int sockbuff_write_varlong(sockbuff_t *buff, int64_t value) {
    uint8_t bytes[10];
    size_t n_bytes = format_varlong(bytes, value);
    return sockbuff_write(buff, bytes, n_bytes);
}
