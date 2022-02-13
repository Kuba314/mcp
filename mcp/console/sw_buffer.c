#include "sw_buffer.h"

#include <stdlib.h>
#include <string.h>

void scrollable_init(scrollable_window_t *sw, WINDOW *win)
{
    sw->win = win;
    sw->buffer_offset = 0;
    sw->msg_count = 0;
}
void scrollable_free(scrollable_window_t *sw)
{
    for(size_t i = 0; i < sw->msg_count; i++) {
        free(sw->buffer[(sw->buffer_offset + i) % SCROLLABLE_WINDOW_BUFFER_SIZE]);
    }
}

int scrollable_push(scrollable_window_t *sw, const char *text, size_t length)
{
    char *new_text = malloc(length + 1);
    if(new_text == NULL) {
        return 1;
    }
    strcpy(new_text, text);

    if(sw->msg_count == SCROLLABLE_WINDOW_BUFFER_SIZE) {
        free(sw->buffer[sw->buffer_offset]);
        sw->buffer[sw->buffer_offset] = new_text;
        sw->buffer_offset = (sw->buffer_offset + 1) % SCROLLABLE_WINDOW_BUFFER_SIZE;
    } else {
        sw->buffer[(sw->buffer_offset + sw->msg_count++) % SCROLLABLE_WINDOW_BUFFER_SIZE] =
            new_text;
    }
    return 0;
}

static int ssign_to_clr(char c)
{
    if(c >= '0' && c <= '9') {
        return c - '0';
    } else if(c >= 'a' && c <= 'f') {
        return c - 'a' + 0xa;
    } else if(c == 'r') {
        return 0xf;
    } else {
        return -1;
    }
}
void scrollable_redraw(scrollable_window_t *sw)
{
    size_t x, y;
    getmaxyx(sw->win, y, x);
    werase(sw->win);

    // draw every message in line buffer that can fit on screen
    for(size_t i = 0; i < y && i < sw->msg_count; i++) {
        const char *msg =
            sw->buffer[(sw->buffer_offset + sw->msg_count - i - 1) % SCROLLABLE_WINDOW_BUFFER_SIZE];
        size_t msg_len = strlen(msg);
        size_t n_printed = 0;

        // print line, while interpreting colors
        wmove(sw->win, y - i - 1, 0);
        bool escaped = false;
        for(size_t i = 0; i < msg_len; i++) {

            // print ellipsis if line doesn't fit
            if(msg_len > x && n_printed >= x - 3) {
                wattrset(sw->win, COLOR_PAIR(0x1f));
                wprintw(sw->win, "%.*s", (int) (x - n_printed), "...");
                break;
            }

            if(escaped) {
                int clr_digit = ssign_to_clr(msg[i]);
                if(clr_digit != -1) {
                    wattron(sw->win, COLOR_PAIR(0x10 + clr_digit));
                }
                escaped = false;
            } else if(strncmp(msg + i, "§", 2) == 0) {
                escaped = true;
                i++; // skip second § char
            } else {
                waddch(sw->win, msg[i]);
                n_printed++;
            }
        }
    }

    // reset color and refresh screen
    wattrset(sw->win, COLOR_PAIR(0x1f));
    wrefresh(sw->win);
}
