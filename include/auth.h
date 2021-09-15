#pragma once

#include <stdint.h>
#include "_string.h"

extern char *g_player_username;
extern char *g_player_password;

int authenticate_with_mojang(string_t *server_id, string_t *pubkey);
