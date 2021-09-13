#pragma once

#include <stddef.h>

int AES_encrypt(unsigned char *dst, const unsigned char *src, size_t length);
int AES_decrypt(unsigned char *dst, const unsigned char *src, size_t length);
