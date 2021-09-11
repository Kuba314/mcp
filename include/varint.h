#pragma once

#include <stdint.h>

#include "unionstream.h"

int read_varint(unionstream_t stream, int32_t *result, uint8_t *n_read);
int read_varlong(unionstream_t stream, int64_t *result, uint8_t *n_read);

uint8_t format_varint(uint8_t bytes[5], uint32_t value);
uint8_t format_varlong(uint8_t bytes[10], uint64_t value);
