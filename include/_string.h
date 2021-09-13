#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *s;
    size_t length;
} string_t;

void print_string(string_t string);
void print_string_hex(string_t string);