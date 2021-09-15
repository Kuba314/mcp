#pragma once

#include <stdint.h>

#include "unionstream.h"

int read_varint(const char *buff, int32_t *result, uint8_t *n_read);
int read_varlong(const char *buff, int64_t *result, uint8_t *n_read);

uint8_t format_varint(char bytes[5], uint32_t value);
uint8_t format_varlong(char bytes[10], uint64_t value);

int read_varint_fd(int sockfd, int32_t *result, uint8_t *n_read);
int read_varlong_fd(int sockfd, int64_t *result, uint8_t *n_read);
