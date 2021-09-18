#include "world.h"

#include <stdlib.h>

#include "tablist.h"
#include "debug.h"

world_t *world = NULL;

int world_init()
{
    world = malloc(sizeof(world_t));
    if(world == NULL) {
        alloc_error();
        return 1;
    }

    world->tablist = hashmap_create(100);
    if(world->tablist == NULL) {
        return 1;
    }

    return 0;
}
