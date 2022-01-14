#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
    char *s;
    size_t length;
} string_t;

typedef struct {
    const char *s;
    size_t length;
} stringview_t;

string_t *string_create(const char *str, size_t length);
string_t *string_alloc(size_t length);
stringview_t stringview_create(const char *str, size_t length);
stringview_t stringview_from_string(string_t *string);

void string_free(string_t *string);

void print_string(string_t *string);
void print_string_hex(string_t *string);

void print_bytes(const void *ptr, size_t length);
void print_bytes_hex(const void *ptr, size_t length);
