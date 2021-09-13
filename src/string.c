#include "_string.h"

#include <stdio.h>

void print_string(string_t string) {
    for(size_t i = 0; i < string.length; i++) {
        fprintf(stderr, "%c", string.s[i]);
    }
}
void print_string_hex(string_t string) {
    for(size_t i = 0; i < string.length; i++) {
        fprintf(stderr, "\\x%02x", string.s[i]);
    }
}
