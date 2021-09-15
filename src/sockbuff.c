#include "sockbuff.h"

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

static int _modify_for_size_increase(sockbuff_t *buff, size_t length)
{
    while(buff->length + length > buff->alloc_length) {
        buff->alloc_length *= 2;
        void *tmp = realloc(buff->data, buff->alloc_length);
        if(tmp == NULL) {
            alloc_error();
            sockbuff_free(buff);
            return 1;
        }

        buff->data = tmp;
    }
    return 0;
}

int sockbuff_write(sockbuff_t *buff, const void *src, size_t length) {
    if(_modify_for_size_increase(buff, length)) {
        return 1;
    }

    memcpy(buff->data + buff->length, src, length);
    buff->length += length;
    return 0;
}

extern int sockbuff_write_byte(sockbuff_t *buff, uint8_t byte);
extern int sockbuff_write_short(sockbuff_t *buff, uint16_t value);
extern int sockbuff_write_c_string(sockbuff_t *buff, const char *src,
                                 size_t length);
extern int sockbuff_write_string(sockbuff_t *buff, string_t *string);
int sockbuff_write_varint(sockbuff_t *buff, int32_t value) {
    char bytes[5];
    size_t n_bytes = format_varint(bytes, value);
    return sockbuff_write(buff, bytes, n_bytes);
}
int sockbuff_write_varlong(sockbuff_t *buff, int64_t value) {
    char bytes[10];
    size_t n_bytes = format_varlong(bytes, value);
    return sockbuff_write(buff, bytes, n_bytes);
}
