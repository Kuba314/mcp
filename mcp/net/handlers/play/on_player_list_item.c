#include "net/packet_handler.h"

#include "data/world.h"

int on_player_list_item(stream_t *stream)
{
    int32_t action;
    if(stream_read_varint(stream, &action)) {
        return 1;
    }

    int32_t num_of_players;
    if(stream_read_varint(stream, &num_of_players)) {
        return 1;
    }
    verbose("player_list", "action: %d, numplayers: %d", action, num_of_players);

    for(int32_t i = 0; i < num_of_players; i++) {
        char uuid[17] = "";
        if(stream_read(stream, uuid, 16))
            return 1;

        switch(action) {
        case 0: {
            string_t *name = stream_read_string(stream);
            if(name == NULL)
                return 1;

            int32_t num_of_properties;
            if(stream_read_varint(stream, &num_of_properties))
                return 1;

            for(int32_t j = 0; j < num_of_properties; j++) {
                string_t *prop_name = stream_read_string(stream);
                if(prop_name == NULL)
                    return 1;

                string_t *prop_value = stream_read_string(stream);
                if(prop_value == NULL)
                    return 1;

                bool is_signed;
                if(stream_read_bool(stream, &is_signed))
                    return 1;

                string_t *signature = NULL;
                if(is_signed) {
                    signature = stream_read_string(stream);
                    if(signature == NULL) {
                        return 1;
                    }
                }

                verbose("onpl", "prop: ");
                verbose("onpl", "%.*s => %.*s (%s)", prop_name->length, prop_name->s,
                        prop_value->length, prop_value->s, is_signed ? "(unsigned)" : signature->s);

                string_free(prop_name);
                string_free(prop_value);
                if(is_signed)
                    string_free(signature);
            }

            int32_t gamemode;
            if(stream_read_varint(stream, &gamemode))
                return 1;

            int32_t ping;
            if(stream_read_varint(stream, &ping))
                return 1;

            bool has_displayname;
            if(stream_read_bool(stream, &has_displayname))
                return 1;

            string_t *displayname = NULL;
            if(has_displayname) {
                displayname = stream_read_string(stream);
                if(displayname == NULL) {
                    return 1;
                }
            }

            if(tablist_add_player(world->tablist, uuid, name->s, gamemode, ping,
                                  (displayname == NULL) ? NULL : displayname->s))
                return 1;

            string_free(name);
            if(has_displayname) {
                string_free(displayname);
            }
            break;
        }
        case 1: {
            int32_t gamemode;
            if(stream_read_varint(stream, &gamemode))
                return 1;

            if(tablist_update_gamemode(world->tablist, uuid, gamemode))
                return 1;
            break;
        }
        case 2: {
            int32_t ping;
            if(stream_read_varint(stream, &ping))
                return 1;

            if(tablist_update_latency(world->tablist, uuid, ping))
                return 1;
            break;
        }
        case 3: {
            bool has_displayname;
            if(stream_read_bool(stream, &has_displayname))
                return 1;

            string_t *displayname = NULL;
            if(has_displayname) {
                displayname = stream_read_string(stream);
                if(displayname == NULL) {
                    return 1;
                }
            }

            if(tablist_update_displayname(world->tablist, uuid, displayname->s))
                return 1;
            break;
        }
        case 4: {
            if(tablist_remove_player(world->tablist, uuid))
                return 1;
            break;
        }
        }
    }
    return 0;
}
