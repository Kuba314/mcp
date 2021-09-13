#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "unionstream.h"

int get_packet_stream(int sockfd, bool compression_enabled, unionstream_t *stream, int32_t *packet_id);
