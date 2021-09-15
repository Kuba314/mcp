#pragma once

#include <stdint.h>
#include "_string.h"

int connect_to_server(const char *ip, uint16_t port);
int authenticate_with_mojang(string_t *server_id, string_t *pubkey);
