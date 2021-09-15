#pragma once

#include <stdio.h>
#include "config.h"

#ifndef NDEBUG

    #define DBG_VERBOSE 2
    #define DBG_DEBUG 1
    #define DBG_INFO 0
    #define DBG_WARNING -1
    #define DBG_ERROR -2

    #define DBG_RED "\033[31;1m"
    #define DBG_GREEN "\033[32;1m"
    #define DBG_BROWN "\033[33m"
    #define DBG_ORANGE "\033[33;1m"
    #define DBG_GRAY "\033[37m"
    #define DBG_GREY "\033[30;1m"
    #define DBG_BLUE "\033[34;1m"
    #define DBG_CYAN "\033[36m"
    #define DBG_RESET "\033[0m"

    #define _debug(level, color, category, msg, ...)                                                                   \
        do {                                                                                                           \
            if(level <= g_verbosity) {                                                                                 \
                fprintf(stderr, "%s:%d [" color "%s" DBG_RESET "] " msg "\n", __FILE__, __LINE__, category,            \
                        ##__VA_ARGS__);                                                                                \
            }                                                                                                          \
        } while(0)
    #define _debug_begin(level, color, category, msg, ...)                                                             \
        do {                                                                                                           \
            if(level <= g_verbosity) {                                                                                 \
                fprintf(stderr, "%s:%d [" color "%s" DBG_RESET "] " msg, __FILE__, __LINE__, category, ##__VA_ARGS__); \
            }                                                                                                          \
        } while(0)
    #define _debug_frag(level, msg, ...)                                                                               \
        do {                                                                                                           \
            if(level <= g_verbosity) {                                                                                 \
                fprintf(stderr, msg, ##__VA_ARGS__);                                                                   \
            }                                                                                                          \
        } while(0)
#else

    #define _debug(level, color, category, msg, ...)
    #define _debug_begin(level, color, category, msg, ...)
    #define _debug_frag(level, msg, ...)
#endif

#define alloc_error() error("alloc", "Failed to allocate memory")

#define error(category, msg, ...) _debug(-2, DBG_RED, category, msg, ##__VA_ARGS__)
#define error_begin(category, msg, ...) _debug_begin(-2, DBG_RED, category, msg, ##__VA_ARGS__)
#define error_frag(msg, ...) _debug_frag(-2, msg, ##__VA_ARGS__)
#define error_bytes(buff, buff_size)                                                                                   \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(-2, "\\x%02x", ((uint8_t *) buff)[i]);                                                         \
        }                                                                                                              \
    } while(0)
#define error_string(buff, buff_size)                                                                                  \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(-2, "%c", ((uint8_t *) buff)[i]);                                                              \
        }                                                                                                              \
    } while(0)
#define error_end() _debug_frag(-2, "\n")

#define warning(category, msg, ...) _debug(-1, DBG_ORANGE, category, msg, ##__VA_ARGS__)
#define warning_begin(category, msg, ...) _debug_begin(-1, DBG_ORANGE, category, msg, ##__VA_ARGS__)
#define warning_frag(msg, ...) _debug_frag(-1, msg, ##__VA_ARGS__)
#define warning_bytes(buff, buff_size)                                                                                 \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(-1, "\\x%02x", ((uint8_t *) buff)[i]);                                                         \
        }                                                                                                              \
    } while(0)
#define warning_string(buff, buff_size)                                                                                \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(-1, "%c", ((uint8_t *) buff)[i]);                                                              \
        }                                                                                                              \
    } while(0)
#define warning_end() _debug_frag(-1, "\n")

#define info(category, msg, ...) _debug(0, DBG_BLUE, category, msg, ##__VA_ARGS__)
#define info_begin(category, msg, ...) _debug_begin(0, DBG_BLUE, category, msg, ##__VA_ARGS__)
#define info_frag(msg, ...) _debug_frag(0, msg, ##__VA_ARGS__)
#define info_bytes(buff, buff_size)                                                                                    \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(0, "\\x%02x", ((uint8_t *) buff)[i]);                                                          \
        }                                                                                                              \
    } while(0)
#define info_string(buff, buff_size)                                                                                   \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(0, "%c", ((uint8_t *) buff)[i]);                                                               \
        }                                                                                                              \
    } while(0)
#define info_end() _debug_frag(0, "\n")

#define debug(category, msg, ...) _debug(1, DBG_CYAN, category, msg, ##__VA_ARGS__)
#define debug_begin(category, msg, ...) _debug_begin(1, DBG_CYAN, category, msg, ##__VA_ARGS__)
#define debug_frag(msg, ...) _debug_frag(1, msg, ##__VA_ARGS__)
#define debug_bytes(buff, buff_size)                                                                                   \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(1, "\\x%02x", ((uint8_t *) buff)[i]);                                                          \
        }                                                                                                              \
    } while(0)
#define debug_string(buff, buff_size)                                                                                  \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(1, "%c", ((uint8_t *) buff)[i]);                                                               \
        }                                                                                                              \
    } while(0)
#define debug_end() _debug_frag(1, "\n")

#define verbose(category, msg, ...) _debug(2, DBG_GRAY, category, msg, ##__VA_ARGS__)
#define verbose_begin(category, msg, ...) _debug_begin(2, DBG_GRAY, category, msg, ##__VA_ARGS__)
#define verbose_frag(msg, ...) _debug_frag(2, msg, ##__VA_ARGS__)
#define verbose_bytes(buff, buff_size)                                                                                 \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(2, "\\x%02x", ((uint8_t *) buff)[i]);                                                          \
        }                                                                                                              \
    } while(0)
#define verbose_string(buff, buff_size)                                                                                \
    do {                                                                                                               \
        for(size_t i = 0; i < buff_size; i++) {                                                                        \
            _debug_frag(2, "%c", ((uint8_t *) buff)[i]);                                                               \
        }                                                                                                              \
    } while(0)
#define verbose_end() _debug_frag(2, "\n")
