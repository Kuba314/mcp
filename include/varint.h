#pragma once

#include <stdint.h>

int read_varint(int sockfd, int32_t *result);
int write_varint(int sockfd, int32_t value);
int read_varlong(int sockfd, int64_t *result);
int write_varlong(int sockfd, int64_t value);
