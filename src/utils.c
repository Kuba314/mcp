#include "utils/utils.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "utils/dynstring.h"
#include "debug.h"

char int2hex(char c)
{
    return c + ((c < 10) ? '0' : 'a' - 10);
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
    bool nonzero_found = false;
    for(size_t i = 0; i < length; i++) {
        hash_hex[i * 2] = hash[i] >> 4;
        hash_hex[i * 2 + 1] = hash[i] & 0xf;

        if(hash_hex[i * 2] == 0 && !nonzero_found) {
            n_zeros++;
            if(hash_hex[i * 2 + 1] == 0) {
                n_zeros++;
                continue;
            }
        }
        nonzero_found = true;
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
    for(int i = sign; i < (int) length * 2 + sign - n_zeros; i++) {
        hash_hex[i] = int2hex(hash_hex[i]);
    }

    *out_length = length * 2 + sign - n_zeros;
    hash_hex[*out_length] = '\0';
    return hash_hex;
}

size_t bufwrite(void *ptr, size_t size, size_t nmemb, buffer_t *buff)
{
    buffer_write(buff, ptr, size * nmemb);
    return size * nmemb;
}

int json_extract_string(json_value *json, string_t **dest, ...)
{
    va_list args;
    va_start(args, dest);

    json_value *obj = json;
    const char *curr = va_arg(args, const char *);
    bool found_key;
    while(curr != NULL) {
        if(obj->type != json_object) {
            error("json", "not an object");
            va_end(args);
            return 1;
        }

        found_key = false;
        for(uint32_t i = 0; i < obj->u.object.length; i++) {
            json_object_entry entry = obj->u.object.values[i];

            if(strncmp(entry.name, curr, entry.name_length) == 0) {
                obj = entry.value;
                curr = va_arg(args, const char *);
                found_key = true;
                break;
            }
        }
        if(!found_key) {
            error("json", "key %s not found in object", curr);
            va_end(args);
            return 1;
        }
    }
    va_end(args);

    if(obj->type != json_string) {
        error("json", "json type is not a string");
        return 1;
    }
    *dest = string_create(obj->u.string.ptr, obj->u.string.length);
    if(*dest == NULL) {
        return 1;
    }
    return 0;
}
