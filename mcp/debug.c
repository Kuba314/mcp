#include "debug.h"

#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "utils/buffer.h"

static const char *get_color_prefix_from_ssign(char c)
{
    static const char *colors[] = {
        "\033[0;30m", "\033[0;34m", "\033[0;32m", "\033[0;36m", "\033[0;31m", "\033[0;35m",
        "\033[0;33m", "\033[0;37m", "\033[1;30m", "\033[1;34m", "\033[1;32m", "\033[1;36m",
        "\033[1;31m", "\033[1;35m", "\033[1;33m", "\033[1;37m",
    };
    if(c >= 'a' && c <= 'f') {
        c = c - 'a' + 10;
    } else if(c >= '0' && c <= '9') {
        c -= '0';
    } else if(c == 'k') { // obfuscated
        return "\033[5m";
    } else if(c == 'l') { // bold
        return "\033[1m";
    } else if(c == 'm') { // strikethrough
        return "\033[9m";
    } else if(c == 'n') { // underline
        return "\033[4m";
    } else if(c == 'o') { // italic
        return "\033[3m";
    } else if(c == 'r') { // reset
        return "\033[0m";
    } else {
        return "";
    }
    return colors[(uint8_t) c];
}

#define BUFF_LENGTH 1048576

void debug_with_colors(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buff[BUFF_LENGTH];
    vsnprintf(buff, BUFF_LENGTH, fmt, args);
    va_end(args);

    bool escaped = false;
    for(uint32_t i = 0; buff[i]; i++) {
        if(escaped) {
            const char *clr_prefix = get_color_prefix_from_ssign(buff[i]);
            fputs(clr_prefix, stderr);
            escaped = false;
        } else if(strncmp(buff + i, "§", 2) == 0) {
            escaped = true;
            i++; // skip second § char
        } else {
            fputc(buff[i], stderr);
        }
    }
}
