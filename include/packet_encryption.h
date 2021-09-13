#pragma once

#include <stddef.h>

int encrypt_AES(unsigned char *dst, const unsigned char *src, size_t length);
int decrypt_AES(unsigned char *dst, const unsigned char *src, size_t length);
