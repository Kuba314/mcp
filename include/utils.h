#pragma once

#include <stdint.h>
#include <stddef.h>

#include "_string.h"
#include "sockbuff.h"

char int2hex(char c);
uint8_t *sha_mc_hexlify(unsigned char *hash, size_t length, size_t *out_length);
size_t bufwrite(void *ptr, size_t size, size_t nmemb, sockbuff_t *buff);

// https://stackoverflow.com/questions/23999797/implementing-strnstr
char *strnstr(const char *haystack, const char *needle, size_t len);

int extract_json_string_pair(const char *buff, size_t buff_len, const char *key_with_quotes, string_t **value);
