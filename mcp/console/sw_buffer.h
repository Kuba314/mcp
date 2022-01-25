#pragma once

#include <ncurses.h>

#define SCROLLABLE_WINDOW_BUFFER_SIZE 1024

typedef struct {
    WINDOW *win;
    char *buffer[SCROLLABLE_WINDOW_BUFFER_SIZE];
    size_t buffer_offset;
    size_t msg_count;
} scrollable_window_t;

void scrollable_init(scrollable_window_t *sw, WINDOW *win);
void scrollable_free(scrollable_window_t *sw);

void scrollable_redraw(scrollable_window_t *sw);
int scrollable_push(scrollable_window_t *sw, const char *text, size_t length);
