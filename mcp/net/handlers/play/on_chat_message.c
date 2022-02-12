#include "net/packet_handler.h"

#include <string.h>
#include <stdint.h>

#include "console/console.h"
#include "utils/buffer.h"
#include "utils/json.h"

static int parse_message_atom_text(buffer_t *buff, json_value *json)
{
    static const char *const clrmap[][2] = {
        {"black",         "\033[0;30m"},
        { "dark_blue",    "\033[0;34m"},
        { "dark_green",   "\033[0;32m"},
        { "dark_aqua",    "\033[0;36m"},
        { "dark_red",     "\033[0;31m"},
        { "dark_purple",  "\033[0;35m"},
        { "gold",         "\033[0;33m"},
        { "gray",         "\033[0;37m"},
        { "dark_gray",    "\033[1;30m"},
        { "blue",         "\033[1;34m"},
        { "green",        "\033[1;32m"},
        { "aqua",         "\033[1;36m"},
        { "red",          "\033[1;31m"},
        { "light_purple", "\033[1;35m"},
        { "yellow",       "\033[1;33m"},
        { "white",        "\033[1;37m"},
    };

    // extract text and color from json object
    stringview_t text = { 0 };
    stringview_t color = { 0 };
    for(uint32_t i = 0; i < json->u.object.length; i++) {
        json_object_entry entry = json->u.object.values[i];

        if(entry.value->type == json_string) {
            if(strcmp(entry.name, "text") == 0) {
                text.s = entry.value->u.string.ptr;
                text.length = entry.value->u.string.length;
            } else if(strcmp(entry.name, "color") == 0) {
                color.s = entry.value->u.string.ptr;
                color.length = entry.value->u.string.length;
            }
        } else if(entry.value->type == json_array) {
            if(strcmp(entry.name, "with") == 0) {
                if(entry.value->u.array.length == 2 && entry.value->u.array.values[1]->type == json_string) {
                    text.s = entry.value->u.array.values[1]->u.string.ptr;
                    text.length = entry.value->u.array.values[1]->u.string.length;
                }
            }
        }
    }

    // write color to buffer if color present
    if(color.s != NULL) {
        bool found = false;
        for(size_t i = 0; i < sizeof(clrmap) / sizeof(*clrmap); i++) {
            if(strcmp(clrmap[i][0], color.s) == 0) {
                found = true;
                const char *clrbytes = clrmap[i][1];
                buffer_write(buff, clrbytes, strlen(clrbytes));
                break;
            }
        }
        if(!found) {
            error("chat", "color is not defined: \"%.*s\"", color.length, color.s);
            return 1;
        }
    }

    // write text to buffer if present
    if(text.s != NULL) {
        buffer_write(buff, text.s, text.length);
    }
    return 0;
}

static int write_message_to_buffer(buffer_t *buff, json_value *json)
{
    json_value *translate = json_extract(json, "translate");
    if(translate != NULL && translate->type == json_string) {
        json_value *with = json_extract(json, "with");
        if(with != NULL && with->type == json_array) {
            if(strcmp(translate->u.string.ptr, "chat.type.text") == 0 && with->u.array.length == 2) {
                json_value *username = json_extract(with->u.array.values[0], "text");
                if(username == NULL) {
                    error("chat", "no username in chat message");
                    return 0;
                } else if(username->type != json_string) {
                    error("chat", "username is not a string");
                    return 0;
                }
                json_value *message = with->u.array.values[1];
                if(message == NULL) {
                    error("chat", "no message in chat message");
                    return 0;
                } else if(username->type != json_string) {
                    error("chat", "message is not a string");
                    return 0;
                }
                buffer_write(buff, "<", 1);
                buffer_write(buff, username->u.string.ptr, username->u.string.length);
                buffer_write(buff, "> ", 2);
                buffer_write(buff, message->u.string.ptr, message->u.string.length);
            } else if(strcmp(translate->u.string.ptr, "death.attack.player") == 0) {
                json_value *username1 = json_extract(with->u.array.values[0], "text");
                if(username1 == NULL) {
                    error("chat", "no username in chat message");
                    return 0;
                } else if(username1->type != json_string) {
                    error("chat", "username is not a string");
                    return 0;
                }
                json_value *username2 = json_extract(with->u.array.values[1], "text");
                if(username2 == NULL) {
                    error("chat", "no username in chat message");
                    return 0;
                } else if(username2->type != json_string) {
                    error("chat", "username is not a string");
                    return 0;
                }
                buffer_write(buff, username1->u.string.ptr, username1->u.string.length);
                buffer_write(buff, " was slain by ", strlen(" was slain by "));
                buffer_write(buff, username2->u.string.ptr, username2->u.string.length);
            }
            return 0;
        }
    }
    // if(json->type != json_object) {
    //     error("json", "chat message json is not an object");
    //     return 1;
    // }
    // const char *translate_as = NULL;
    // json_value **with = NULL;
    // uint32_t with_length = NULL;
    // for(uint32_t i = 0; i < json->u.object.length; i++) {
    //     json_object_entry entry = json->u.object.values[i];

    //     if(strcmp(entry.name, "translate") == 0 && entry.value->type == json_string) {
    //         translate_as = entry.value->u.string.ptr;
    //         break;
    //     } else if(strcmp(entry.name, "with") == 0 && entry.value->type == json_array) {
    //         with = entry.value->u.array.values;
    //         with_length = entry.value->u.array.length;
    //     }
    // }
    // if(translate_as != NULL && ) {
    //     if(strcmp(translate_as, "chat.type.text") == 0) {
    //         if(with_length != 2) {
    //             return 1;
    //         }
    //         json_extract_string()

    //     } else if(strcmp(translate_as, "death.attack.player") == 0) {

    //     }
    // }

    if(parse_message_atom_text(buff, json)) {
        json_value_free(json);
        buffer_free(buff);
        return 1;
    }
    for(uint32_t i = 0; i < json->u.object.length; i++) {
        json_object_entry entry = json->u.object.values[i];

        if(strncmp(entry.name, "extra", entry.name_length) == 0) {
            if(entry.value->type != json_array) {
                error("json", "\"extra\" value is not array, but %d", entry.value->type);
                json_value_free(json);
                buffer_free(buff);
                return 1;
            }

            for(uint32_t j = 0; j < entry.value->u.array.length; j++) {
                json_value *extra_el = entry.value->u.array.values[j];
                if(parse_message_atom_text(buff, extra_el)) {
                    json_value_free(json);
                    buffer_free(buff);
                    return 1;
                }
            }
        }
    }
    return 0;
}

static int print_chat_message(string_t *json)
{
    verbose("chat", "%.*s", json->length, json->s);
    json_value *parsed = json_parse(json->s, json->length);
    if(parsed == NULL) {
        error("chat", "invalid chat json: %s", json->s);
        return 1;
    }

    if(parsed->type != json_object) {
        error("chat", "invalid chat json type(%d): %s", parsed->type, json->s);
        json_value_free(parsed);
        return 1;
    }

    buffer_t *buff = buffer_create();
    if(buff == NULL) {
        json_value_free(parsed);
        return 1;
    }

    write_message_to_buffer(buff, parsed);
    json_value_free(parsed);

    if(buff->length == 0) {
        return 0;
    }

    if(console_is_running()) {
        console_chat(buff->data);
    } else {
        info("chat", "%s", buff->data);
    }

    buffer_free(buff);
    return 0;
}

int on_chat_message(stream_t *stream)
{
    string_t *json_data = stream_read_string(stream);
    if(json_data == NULL) {
        return 1;
    }

    int8_t position;
    if(stream_read_byte(stream, &position)) {
        return 1;
    }

    if(print_chat_message(json_data)) {
        return 1;
    }

    // switch(position) {
    // case 0: info("chat", "%s", json_data->s); break;
    // case 1: info("sysmsg", "%s", json_data->s); break;
    // case 2: info("hotbmsg", "%s", json_data->s); break;
    // }

    string_free(json_data);
    return 0;
}
