#pragma once

#include <stdio.h>
#include "config.h"

#ifndef NDEBUG

    #define DBG_ERROR 50
    #define DBG_WARN 40
    #define DBG_INFO 30
    #define DBG_DEBUG 20
    #define DBG_VERBOSE 10

    #define CLR_RED "\033[31;1m"
    #define CLR_GREEN "\033[32;1m"
    #define CLR_BROWN "\033[33m"
    #define CLR_ORANGE "\033[33;1m"
    #define CLR_GRAY "\033[37m"
    #define CLR_GREY "\033[30;1m"
    #define CLR_BLUE "\033[34;1m"
    #define CLR_CYAN "\033[36m"
    #define CLR_RESET "\033[0m"

    #define debug_with_level(level, color, category, msg, ...)                                     \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                fprintf(stderr, "%s:%d [" color "%s" CLR_RESET "] " msg "\n", __FILE__, __LINE__,  \
                        category, ##__VA_ARGS__);                                                  \
            }                                                                                      \
        } while(0)
    #define debug_begin_with_level(level, color, category, msg, ...)                               \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                fprintf(stderr, "%s:%d [" color "%s" CLR_RESET "] " msg, __FILE__, __LINE__,       \
                        category, ##__VA_ARGS__);                                                  \
            }                                                                                      \
        } while(0)
    #define debug_frag_with_level(level, msg, ...)                                                 \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                fprintf(stderr, msg, ##__VA_ARGS__);                                               \
            }                                                                                      \
        } while(0)
    #define debug_bytes_with_level(level, buff, buff_size)                                         \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                for(size_t i = 0; i < buff_size; i++) {                                            \
                    debug_frag_with_level(level, "\\x%02x", ((uint8_t *) buff)[i]);                \
                }                                                                                  \
            }                                                                                      \
        } while(0)
    #define debug_string_with_level(level, buff, buff_size)                                        \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                for(size_t i = 0; i < buff_size; i++) {                                            \
                    debug_frag_with_level(level, "%c", ((uint8_t *) buff)[i]);                     \
                }                                                                                  \
            }                                                                                      \
        } while(0)
    #define debug_end_with_level(level)                                                            \
        do {                                                                                       \
            if(level >= g_verbosity) {                                                             \
                fprintf(stderr, "\n");                                                             \
            }                                                                                      \
        } while(0)
#else
    #define debug_with_level(level, color, category, msg, ...)
    #define debug_begin_with_level(level, color, category, msg, ...)
    #define debug_frag_with_level(level, msg, ...)
    #define debug_bytes_with_level(level, buff, buff_size)
    #define debug_string_with_level(level, buff, buff_size)
    #define debug_end_with_level(level)
#endif

#define alloc_error() error("alloc", "Failed to allocate memory")

#define error(category, msg, ...) debug_with_level(DBG_ERROR, CLR_RED, category, msg, ##__VA_ARGS__)
#define error_begin(category, msg, ...)                                                            \
    debug_begin_with_level(DBG_ERROR, CLR_RED, category, msg, ##__VA_ARGS__)
#define error_frag(msg, ...) debug_frag_with_level(DBG_ERROR, msg, ##__VA_ARGS__)
#define error_bytes(buff, buff_size) debug_bytes_with_level(DBG_ERROR, buff, buff_size)
#define error_string(buff, buff_size) debug_string_with_level(DBG_ERROR, buff, buff_size)
#define error_end() debug_end_with_level(DBG_ERROR)

#define warn(category, msg, ...)                                                                   \
    debug_with_level(DBG_WARN, CLR_ORANGE, category, msg, ##__VA_ARGS__)
#define warn_begin(category, msg, ...)                                                             \
    debug_begin_with_level(DBG_WARN, CLR_ORANGE, category, msg, ##__VA_ARGS__)
#define warn_frag(msg, ...) debug_frag_with_level(DBG_WARN, msg, ##__VA_ARGS__)
#define warn_bytes(buff, buff_size) debug_bytes_with_level(DBG_WARN, buff, buff_size)
#define warn_string(buff, buff_size) debug_string_with_level(DBG_WARN, buff, buff_size)
#define warn_end() debug_end_with_level(DBG_WARN)

#define info(category, msg, ...) debug_with_level(DBG_INFO, CLR_BLUE, category, msg, ##__VA_ARGS__)
#define info_begin(category, msg, ...)                                                             \
    debug_begin_with_level(DBG_INFO, CLR_BLUE, category, msg, ##__VA_ARGS__)
#define info_frag(msg, ...) debug_frag_with_level(DBG_INFO, msg, ##__VA_ARGS__)
#define info_bytes(buff, buff_size) debug_bytes_with_level(DBG_INFO, buff, buff_size)
#define info_string(buff, buff_size) debug_string_with_level(DBG_INFO, buff, buff_size)
#define info_end() debug_end_with_level(DBG_INFO)

#define debug(category, msg, ...)                                                                  \
    debug_with_level(DBG_DEBUG, CLR_CYAN, category, msg, ##__VA_ARGS__)
#define debug_begin(category, msg, ...)                                                            \
    debug_begin_with_level(DBG_DEBUG, CLR_CYAN, category, msg, ##__VA_ARGS__)
#define debug_frag(msg, ...) debug_frag_with_level(DBG_DEBUG, msg, ##__VA_ARGS__)
#define debug_bytes(buff, buff_size) debug_bytes_with_level(DBG_DEBUG, buff, buff_size)
#define debug_string(buff, buff_size) debug_string_with_level(DBG_DEBUG, buff, buff_size)
#define debug_end() debug_end_with_level(DBG_DEBUG)

#define verbose(category, msg, ...)                                                                \
    debug_with_level(DBG_VERBOSE, CLR_GRAY, category, msg, ##__VA_ARGS__)
#define verbose_begin(category, msg, ...)                                                          \
    debug_begin_with_level(DBG_VERBOSE, CLR_GRAY, category, msg, ##__VA_ARGS__)
#define verbose_frag(msg, ...) debug_frag_with_level(DBG_VERBOSE, msg, ##__VA_ARGS__)
#define verbose_bytes(buff, buff_size) debug_bytes_with_level(DBG_VERBOSE, buff, buff_size)
#define verbose_string(buff, buff_size) debug_string_with_level(DBG_VERBOSE, buff, buff_size)
#define verbose_end() debug_end_with_level(DBG_VERBOSE)
