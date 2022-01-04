#include "packet_handler.h"

#include <string.h>
#include <stdint.h>

#include "sockbuff.h"
#include "json.h"

static const char *get_color_prefix_from_ssign(char c)
{
    static const char *colors[] = {
        "\033[0;30m",
        "\033[0;34m",
        "\033[0;32m",
        "\033[0;36m",
        "\033[0;31m",
        "\033[0;35m",
        "\033[0;33m",
        "\033[0;37m",
        "\033[1;30m",
        "\033[1;34m",
        "\033[1;32m",
        "\033[1;36m",
        "\033[1;31m",
        "\033[1;35m",
        "\033[1;33m",
        "\033[1;37m",
    };
    if(c >= 'a' && c <= 'f') {
        c = c - 'a' + 10;
    } else if(c >= '0' && c <= '9') {
        c -= '0';
    } else if(c == 'k') {   // obfuscated
        return "\033[5m";
    } else if(c == 'l') {   // bold
        return "\033[1m";
    } else if(c == 'm') {   // strikethrough
        return "\033[9m";
    } else if(c == 'n') {   // underline
        return "\033[4m";
    } else if(c == 'o') {   // italic
        return "\033[3m";
    } else if(c == 'r') {   // reset
        return "\033[0m";
    } else {
        return "";
    }
    return colors[(uint8_t) c];
}

static int parse_message_atom_text(sockbuff_t *buff, json_value *json)
{
    static const char *const clrmap[][2] = {
        { "black",        "\033[0;30m", },
        { "dark_blue",    "\033[0;34m", },
        { "dark_green",   "\033[0;32m", },
        { "dark_aqua",    "\033[0;36m", },
        { "dark_red",     "\033[0;31m", },
        { "dark_purple",  "\033[0;35m", },
        { "gold",         "\033[0;33m", },
        { "gray",         "\033[0;37m", },
        { "dark_gray",    "\033[1;30m", },
        { "blue",         "\033[1;34m", },
        { "green",        "\033[1;32m", },
        { "aqua",         "\033[1;36m", },
        { "red",          "\033[1;31m", },
        { "light_purple", "\033[1;35m", },
        { "yellow",       "\033[1;33m", },
        { "white",        "\033[1;37m", },
    };
    stringview_t text = { 0 };
    stringview_t color = { 0 };
    for(uint32_t i = 0; i < json->u.object.length; i++) {
        json_object_entry entry = json->u.object.values[i];

        if(entry.value->type != json_string)
            continue;

        if(strncmp(entry.name, "text", entry.name_length) == 0) {
            text.s = entry.value->u.string.ptr;
            text.length = entry.value->u.string.length;
        } else if(strncmp(entry.name, "color", entry.name_length) == 0) {
            color.s = entry.value->u.string.ptr;
            color.length = entry.value->u.string.length;
        } else {
            debug("json_key", "%s", entry.name);
        }
    }

    if(color.s != NULL) {
        bool found = false;
        for(size_t i = 0; i < sizeof(clrmap) / sizeof(*clrmap); i++) {
            if(strcmp(clrmap[i][0], color.s) == 0) {
                found = true;
                const char *clrbytes = clrmap[i][1];
                sockbuff_write(buff, clrbytes, strlen(clrbytes));
                break;
            }
        }
        if(!found) {
            error_begin("chat", "color is not defined: \"");
            print_bytes(color.s, color.length);
            error_frag("\"");
            error_end();
            return 1;
        }
    }
    if(text.s != NULL) {
        bool escaped = false;
        for(uint32_t i = 0; i < text.length; i++) {
            if(escaped) {
                const char *clr_prefix = get_color_prefix_from_ssign(text.s[i]);
                sockbuff_write(buff, clr_prefix, strlen(clr_prefix));
                escaped = false;
            } else if(strncmp(text.s + i, "§", 2) == 0) {
                escaped = true;
                i++;    // skip second § char
            } else {
                sockbuff_write(buff, &text.s[i], 1);
            }
        }
    }
    sockbuff_write(buff, "\033(B\033[m", strlen("\033(B\033[m"));
    return 0;
}
static int print_chat_message(string_t *json)
{
    json_value *parsed = json_parse(json->s, json->length);
    if(parsed == NULL) {
        error("json", "invalid chat json: %s", json->s);
        return 1;
    }

    if(parsed->type != json_object) {
        error("json", "invalid chat json type(%d): %s", parsed->type, json->s);
        json_value_free(parsed);
        return 1;
    }

    sockbuff_t *buff = sockbuff_create();
    if(buff == NULL) {
        json_value_free(parsed);
        return 1;
    }

    if(parse_message_atom_text(buff, parsed)) {
        json_value_free(parsed);
        sockbuff_free(buff);
        return 1;
    }
    for(uint32_t i = 0; i < parsed->u.object.length; i++) {
        json_object_entry entry = parsed->u.object.values[i];

        if(strncmp(entry.name, "extra", entry.name_length) == 0) {
            if(entry.value->type != json_array) {
                error("json", "\"extra\" value is not array, but %d", entry.value->type);
                json_value_free(parsed);
                sockbuff_free(buff);
                return 1;
            }

            for(uint32_t j = 0; j < entry.value->u.array.length; j++) {
                json_value *extra_el = entry.value->u.array.values[j];
                if(parse_message_atom_text(buff, extra_el)) {
                    json_value_free(parsed);
                    sockbuff_free(buff);
                    return 1;
                }
            }
        }
    }
    json_value_free(parsed);

    info_begin("chat", "");
    print_bytes(buff->data, buff->length);
    info_end();

    sockbuff_free(buff);
    return 0;
}

int on_chat_message(unionstream_t *stream)
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
