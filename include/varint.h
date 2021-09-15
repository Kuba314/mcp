#pragma once

#include <stdint.h>

#include "unionstream.h"

int read_varint(const uint8_t *buff, int32_t *result, uint8_t *n_read);
int read_varlong(const uint8_t *buff, int64_t *result, uint8_t *n_read);

uint8_t format_varint(uint8_t bytes[5], int32_t value);
uint8_t format_varlong(uint8_t bytes[10], int64_t value);

int read_varint_fd(int sockfd, int32_t *result, uint8_t *n_read);
int read_varlong_fd(int sockfd, int64_t *result, uint8_t *n_read);
