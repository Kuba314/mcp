#pragma once

#include <stdio.h>

#ifndef NDEBUG

    #define DBG_RED    "\033[31m"
    #define DBG_GREEN  "\033[32;1m"
    #define DBG_BROWN  "\033[33m"
    #define DBG_ORANGE "\033[33;1m"
    #define DBG_GRAY   "\033[37m"
    #define DBG_RESET  "\033[0m"

    #define _debug(color, category, msg, ...) fprintf(stderr, "%s:%d [" color "%s" DBG_RESET "] " msg "\n", __FILE__, __LINE__, category, ##__VA_ARGS__)
    #define _debug_begin(color, category, msg, ...) fprintf(stderr, "%s:%d [" color "%s" DBG_RESET "] " msg, __FILE__, __LINE__, category, ##__VA_ARGS__)
    #define _debug_frag(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)

    #define error(category, msg, ...) _debug(DBG_RED, category, msg, ##__VA_ARGS__)
    #define error_begin(category, msg, ...) _debug_begin(DBG_RED, category, msg, ##__VA_ARGS__)
    #define error_frag(msg, ...) _debug_frag(msg, ##__VA_ARGS__)
    #define error_end() fprintf(stderr, "\n")

    #define warning(category, msg, ...) _debug(DBG_GREEN, category, msg, ##__VA_ARGS__)
    #define warning_begin(category, msg, ...) _debug_begin(DBG_GREEN, category, msg, ##__VA_ARGS__)
    #define warning_frag(msg, ...) _debug_frag(msg, ##__VA_ARGS__)
    #define warning_end() fprintf(stderr, "\n")

    #define info(category, msg, ...) _debug(DBG_GREEN, category, msg, ##__VA_ARGS__)
    #define info_begin(category, msg, ...) _debug_begin(DBG_GREEN, category, msg, ##__VA_ARGS__)
    #define info_frag(msg, ...) _debug_frag(msg, ##__VA_ARGS__)
    #define info_end() fprintf(stderr, "\n")

    #define debug(category, msg, ...) _debug(DBG_BROWN, category, msg, ##__VA_ARGS__)
    #define debug_begin(category, msg, ...) _debug_begin(DBG_BROWN, category, msg, ##__VA_ARGS__)
    #define debug_frag(msg, ...) _debug_frag(msg, ##__VA_ARGS__)
    #define debug_end() fprintf(stderr, "\n")

    #define verbose(category, msg, ...) _debug(DBG_GRAY, category, msg, ##__VA_ARGS__)
    #define verbose_begin(category, msg, ...) _debug_begin(DBG_GRAY, category, msg, ##__VA_ARGS__)
    #define verbose_frag(msg, ...) _debug_frag(msg, ##__VA_ARGS__)
    #define verbose_end() fprintf(stderr, "\n")
#else
    #define _debug(...)
    #define _debug_begin(...)
    #define _debug_frag(...)
#endif
