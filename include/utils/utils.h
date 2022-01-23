#pragma once

#include <stdint.h>
#include <stddef.h>

#include "utils/dynstring.h"
#include "utils/json.h"
#include "utils/buffer.h"

char int2hex(char c);
uint8_t *sha_mc_hexlify(unsigned char *hash, size_t length, size_t *out_length);
size_t bufwrite(void *ptr, size_t size, size_t nmemb, buffer_t *buff);

int json_extract_string(json_value *json, string_t **dest, ...);
