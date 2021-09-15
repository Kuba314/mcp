#include "utils.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "_string.h"
#include "debug.h"

char int2hex(char c)
{
    return c + ((c < 10) ? '0': 'a' - 10);
}

uint8_t *sha_mc_hexlify(unsigned char *hash, size_t length, size_t *out_length)
{

    // negative hash and null byte
    uint8_t *hash_hex = malloc(length * 2 + 2);
    if(hash_hex == NULL) {
        alloc_error();
        return NULL;
    }

    // space out the chars, count zeros
    int n_zeros = 0;
    int nonzero_found = 0;
    for(size_t i = 0; i < length; i++) {
        hash_hex[i * 2]     = hash[i] >> 4;
        hash_hex[i * 2 + 1] = hash[i] & 0xf;

        if(hash_hex[i * 2] == 0 && nonzero_found) {
            n_zeros++;
            if(hash_hex[i * 2 + 1] == 0) {
                n_zeros++;
                continue;
            }
        }
        nonzero_found = 1;
    }

    int sign = hash[0] >> 7;
    if(sign) {
        int carry = 0;
        for(int i = 2 * length - 1; i >= 0; i--) {

            unsigned char new_c = (hash_hex[i] ^ 0xf) + carry;
            if(i == (int) length * 2 - 1) {
                new_c++;
            }
            carry = new_c >> 4;
            hash_hex[i] = new_c & 0xf;
        }
    }

    // move correctly
    if(sign || n_zeros) {
        memmove(hash_hex + sign, hash_hex + n_zeros, length * 2 - n_zeros);
        if(sign) {
            hash_hex[0] = '-';
        }
    }

    // convert to readable format
    for(int i = sign; i < (int) length * 2 + sign- n_zeros; i++) {
        hash_hex[i] = int2hex(hash_hex[i]);
    }

    *out_length = length * 2 + sign - n_zeros;
    hash_hex[*out_length] = '\0';
    return hash_hex;
}

size_t bufwrite(void *ptr, size_t size, size_t nmemb, sockbuff_t *buff)
{
    sockbuff_write(buff, ptr, size * nmemb);
    return size*nmemb;
}

// https://stackoverflow.com/questions/23999797/implementing-strnstr
char *strnstr(const char *haystack, const char *needle, size_t len)
{
    size_t needle_len = strnlen(needle, len);

    if(needle_len == 0) {
        return (char *) haystack;
    }

    for(size_t i = 0; i <= len - needle_len; i++) {
        if(*haystack == *needle && strncmp(haystack, needle, needle_len) == 0) {
            return (char *) haystack;
        }
        haystack++;
    }
    return NULL;
}
int find_closing_quote(const char *str, size_t length)
{
    bool escaped = false;
    str++;

    for(size_t i = 0; i < length; i++) {
        if(escaped) {
            continue;
        } else if(str[i] == '\\') {
            escaped = true;
        } else if(str[i] == '"') {
            return i;
        }
    }
    return -1;
}
int extract_json_string_pair(const char *buff, size_t buff_len, const char *key_with_quotes, string_t **value)
{
    char *token = strnstr(buff, key_with_quotes, buff_len);
    if(token == NULL) {
        error_begin("json", "key %s not found in ", key_with_quotes);
        print_bytes(buff, buff_len);
        error_end();
        return 1;
    }

    // skip whitespace and a colon
    token += strlen(key_with_quotes);
    while(isspace(*token) || *token == ':') {
        token++;
    }
    int offset = find_closing_quote(token, buff_len - (token - buff));
    if(offset == -1) {
        error_begin("json", "malformed json string: ");
        print_bytes(buff, buff_len);
        error_end();
        return 1;
    }

    *value = string_create(token + 1, offset);
    return 0;
}
