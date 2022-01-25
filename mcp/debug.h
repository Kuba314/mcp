#pragma once

#include <stdio.h>
#include "config.h"
#include "console/console.h"
#include <ncurses.h>

void debug_with_colors(const char *fmt, ...);

#ifndef NDEBUG

    #define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)

    #define DBG_ERROR 50
    #define DBG_WARN 40
    #define DBG_INFO 30
    #define DBG_DEBUG 20
    #define DBG_VERBOSE 10

    #define CLR_RED "§c"
    #define CLR_ORANGE "§e"
    #define CLR_GRAY "§7"
    #define CLR_GREY "§8"
    #define CLR_BLUE "§9"
    #define CLR_CYAN "§3"
    #define CLR_RESET "§r"

    #define debug_with_level(level, color, category, msg, ...)                                     \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                if(stdscr == NULL || !g_console_enabled) {                                         \
                    debug_with_colors("[" color "%s" CLR_RESET "] " msg "\n", category,            \
                                      ##__VA_ARGS__);                                              \
                } else {                                                                           \
                    console_debug("[" color "%s" CLR_RESET "] " msg, category, ##__VA_ARGS__);     \
                }                                                                                  \
            }                                                                                      \
        } while(0)
#else
    #define debug_with_level(level, color, category, msg, ...)
#endif

#define alloc_error() error("alloc", "Failed to allocate memory")

#define error(category, msg, ...) debug_with_level(DBG_ERROR, CLR_RED, category, msg, ##__VA_ARGS__)
#define warn(category, msg, ...)                                                                   \
    debug_with_level(DBG_WARN, CLR_ORANGE, category, msg, ##__VA_ARGS__)
#define info(category, msg, ...) debug_with_level(DBG_INFO, CLR_BLUE, category, msg, ##__VA_ARGS__)
#define debug(category, msg, ...)                                                                  \
    debug_with_level(DBG_DEBUG, CLR_CYAN, category, msg, ##__VA_ARGS__)
#define verbose(category, msg, ...)                                                                \
    debug_with_level(DBG_VERBOSE, CLR_GRAY, category, msg, ##__VA_ARGS__)
