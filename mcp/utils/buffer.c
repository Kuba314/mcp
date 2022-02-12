#include "utils/buffer.h"

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "data/varint.h"
#include "debug.h"

buffer_t *buffer_create()
{
    buffer_t *buff = malloc(sizeof(buffer_t));
    if(buff == NULL) {
        alloc_error();
        return NULL;
    }

    buff->data = malloc(SOCKBUFF_INITIAL_SIZE);
    if(buff->data == NULL) {
        alloc_error();
        free(buff);
        return NULL;
    }

    buff->length = 0;
    buff->alloc_length = SOCKBUFF_INITIAL_SIZE;
    return buff;
}
void buffer_free(buffer_t *buff)
{
    if(buff->data != NULL) {
        free(buff->data);
    }
    free(buff);
}

int buffer_write(buffer_t *buff, const void *src, size_t length)
{
    while(buff->length + length + 1 > buff->alloc_length) {
        buff->alloc_length *= 2;
        void *tmp = realloc(buff->data, buff->alloc_length + 1);
        if(tmp == NULL) {
            alloc_error();
            buffer_free(buff);
            return 1;
        }

        buff->data = tmp;
    }

    memcpy(buff->data + buff->length, src, length);
    buff->length += length;
    buff->data[buff->length] = '\0'; // null byte cuz why not
    return 0;
}

extern int buffer_write_byte(buffer_t *buff, uint8_t byte);
extern int buffer_write_short(buffer_t *buff, uint16_t value);
extern int buffer_write_c_string(buffer_t *buff, const char *src);
extern int buffer_write_string(buffer_t *buff, string_t *string);

int buffer_write_varint(buffer_t *buff, int32_t value)
{
    uint8_t bytes[5];
    size_t n_bytes = format_varint(bytes, value);
    return buffer_write(buff, bytes, n_bytes);
}
int buffer_write_varlong(buffer_t *buff, int64_t value)
{
    uint8_t bytes[10];
    size_t n_bytes = format_varlong(bytes, value);
    return buffer_write(buff, bytes, n_bytes);
}
