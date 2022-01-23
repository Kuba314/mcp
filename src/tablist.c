#include "data/tablist.h"

#include <stdlib.h>

#include "debug.h"

int tablist_add_player(tablist_t *tablist, const char *uuid, char *name, int32_t gamemode,
                       int32_t ping, char *displayname)
{
    tablist_item_t *list_item = malloc(sizeof(tablist_item_t));
    list_item->name = name;
    // list_item->properties = properties;
    list_item->gamemode = gamemode;
    list_item->ping = ping;
    list_item->displayname = displayname;
    debug("tablist", "add player %s gm(%d) ping(%d) dispname(%s)", name, gamemode, ping,
          (displayname == NULL) ? "(nil)" : displayname);
    return hashmap_put(tablist, uuid, list_item);
}
int tablist_remove_player(tablist_t *tablist, char *uuid)
{
    if(hashmap_remove(tablist, uuid) != NULL) {
        return 0;
    } else {
        error("tablist", "player with uuid \"%s\" not found", uuid);
        return 1;
    }
}
int tablist_update_gamemode(tablist_t *tablist, const char *uuid, int32_t gamemode)
{
    tablist_item_t *player = hashmap_find(tablist, uuid);
    if(player == NULL) {
        return 1;
    }
    player->gamemode = gamemode;
    return 0;
}
int tablist_update_latency(tablist_t *tablist, const char *uuid, int32_t ping)
{
    tablist_item_t *player = hashmap_find(tablist, uuid);
    if(player == NULL) {
        return 1;
    }
    player->ping = ping;
    return 0;
}
int tablist_update_displayname(tablist_t *tablist, const char *uuid, char *displayname)
{
    tablist_item_t *player = hashmap_find(tablist, uuid);
    if(player == NULL) {
        return 1;
    }
    player->displayname = displayname;
    return 0;
}
void tablist_free(tablist_t *tablist)
{
    hashmap_free(tablist);
}
