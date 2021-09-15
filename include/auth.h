#pragma once

#include <stdint.h>
#include "_string.h"

extern char *g_player_username;
extern char *g_player_password;

int mojang_authenticate(const char *username, const char *password, string_t **auth_token, string_t **uuid);
