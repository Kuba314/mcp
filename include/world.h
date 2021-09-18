#pragma once

#include "tablist.h"

typedef struct {
    tablist_t *tablist;
} world_t;

int world_init();

extern world_t *world;
