#include "_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

string_t *string_alloc(size_t length)
{
    string_t *string = malloc(sizeof(string_t));
    if(string == NULL) {
        alloc_error();
        return NULL;
    }

    string->s = malloc(length + 1);
    if(string->s == NULL) {
        alloc_error();
        return NULL;
    }
    string->length = length;
    return string;
}
string_t *string_create(const char *str, size_t size)
{
    string_t *string = string_alloc(size);
    if(string == NULL) {
        return NULL;
    }
    memcpy(string->s, str, size);
    string->s[size] = '\0';
    return string;
}
stringview_t stringview_create(const char *str, size_t length)
{
    return (stringview_t) {.s = str, .length = length};
}
stringview_t stringview_from_string(string_t *string)
{
    return (stringview_t) {.s = string->s, .length = string->length};
}

void string_free(string_t *string) {
    if(string->s != NULL) {
        free(string->s);
    }
    free(string);
}

void print_string(string_t *string) {
    print_string_c(string->s, string->length);
}
void print_string_hex(string_t *string) {
    print_string_c_hex(string->s, string->length);
}

void print_string_c(const char *str, size_t length)
{
    for(size_t i = 0; i < length; i++) {
        fprintf(stderr, "%c", (unsigned char) str[i]);
    }
}
void print_string_c_hex(const char *str, size_t length)
{
    for(size_t i = 0; i < length; i++) {
        fprintf(stderr, "\\x%02x", (unsigned char) str[i]);
    }
}
