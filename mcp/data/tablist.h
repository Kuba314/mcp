#pragma once

#include "utils/hashmap.h"
#include <stdint.h>
// #include "linked_list.h"

typedef struct {
    char *name;
    char *value;
    char *signature;
} tablist_item_property_t;

typedef struct {
    char *uuid;
    char *name;
    // linked_list_t *properties;
    int32_t gamemode;
    int32_t ping;
    char *displayname;
} tablist_item_t;

typedef hashmap_t tablist_t;

int tablist_add_player(tablist_t *tablist, const char *uuid, char *name, int32_t gamemode,
                       int32_t ping, char *displayname);
int tablist_remove_player(tablist_t *tablist, char *uuid);
int tablist_update_gamemode(tablist_t *tablist, const char *uuid, int32_t gamemode);
int tablist_update_latency(tablist_t *tablist, const char *uuid, int32_t ping);
int tablist_update_displayname(tablist_t *tablist, const char *uuid, char *displayname);
void tablist_free(tablist_t *tablist);
