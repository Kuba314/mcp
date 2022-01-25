#pragma once

#include "data/tablist.h"

typedef struct {
    tablist_t *tablist;
} world_t;

int world_init();
void world_free();

extern world_t *world;
