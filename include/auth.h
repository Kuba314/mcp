#pragma once

#include <stdint.h>
#include "utils/dynstring.h"

int authenticate_with_mojang(string_t *server_id, string_t *pubkey);
