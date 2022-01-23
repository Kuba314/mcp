#include "console.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <ncurses.h>
#include <form.h>

#define SCROLLABLE_WINDOW_BUFFER_SIZE 1024
typedef struct {
    WINDOW *win;
    char *buffer[SCROLLABLE_WINDOW_BUFFER_SIZE];
    size_t buffer_offset;
    size_t msg_count;
} scrollable_window_t;

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
static void redraw_scrollable(scrollable_window_t *sw)
{
    size_t x, y;
    getmaxyx(sw->win, y, x);
    werase(sw->win);
    box(sw->win, 0, 0);
    for(size_t i = 0; i < y - 2 && i < sw->msg_count; i++) {
        const char *msg = sw->buffer[(sw->buffer_offset + sw->msg_count - i - 1) % SCROLLABLE_WINDOW_BUFFER_SIZE];
        size_t msg_len = strlen(msg);
        if(msg_len > x - 2) {
            mvwprintw(sw->win, y - i - 2, 1, "%.*s", x - 5, msg);
            wprintw(sw->win, "...");
        } else {
            mvwprintw(sw->win, y - i - 2, 1, "%s", msg);
        }
    }
    wrefresh(sw->win);
}
int scrollable_push(scrollable_window_t *sw, const char *text)
{
    size_t text_len = strlen(text);
    char *new_text = malloc(text_len + 1);
    if(new_text == NULL) {
        return 1;
    }
    strcpy(new_text, text);

    if(sw->msg_count == SCROLLABLE_WINDOW_BUFFER_SIZE) {
        free(sw->buffer[sw->buffer_offset]);
        sw->buffer[sw->buffer_offset] = new_text;
        sw->buffer_offset = (sw->buffer_offset + 1) % SCROLLABLE_WINDOW_BUFFER_SIZE;
    } else {
        sw->buffer[(sw->buffer_offset + sw->msg_count++) % SCROLLABLE_WINDOW_BUFFER_SIZE] = new_text;
    }

    redraw_scrollable(sw);
    return 0;
}

typedef struct {
    WINDOW *input_type_win;
    WINDOW *input_win;
    scrollable_window_t dbg_win;
    scrollable_window_t chat_win;
    FORM *input_form;
    FIELD *fields[2];
    bool cmd_mode;

    bool running;
} cctx_t;

static cctx_t ctx;

static void cctx_set_cmd_mode(bool cmd_mode)
{
    ctx.cmd_mode = cmd_mode;
    mvwaddch(ctx.input_type_win, 0, 0, (cmd_mode) ? '/' : '>');
    wrefresh(ctx.input_type_win);
}
static char *trim_whitespace(char *data)
{
    while(isspace(*data)) {
        data++;
    }

    if(*data == '\0') {
        return data;
    }

    char *begin = data;
    char *end = data;
    while(*data) {
        if(!isspace(*data)) {
            end = data;
        }
        data++;
    }
    *(end + 1) = '\0';
    return begin;
}
void cctx_redraw_all(void)
{
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    wresize(ctx.dbg_win.win, maxY - 1, maxX / 2);
    mvwin(ctx.dbg_win.win, 0, 0);

    wresize(ctx.chat_win.win, maxY - 1, maxX / 2);
    mvwin(ctx.chat_win.win, 0, maxX / 2);

    wresize(ctx.input_type_win, 1, 1);
    mvwin(ctx.input_type_win, maxY - 1, 0);

    wresize(ctx.input_win, 1, maxX - 2);
    mvwin(ctx.input_win, maxY - 1, 2);

    erase();
    refresh();

    redraw_scrollable(&ctx.dbg_win);
    redraw_scrollable(&ctx.chat_win);

    mvwaddch(ctx.input_type_win, 0, 0, (ctx.cmd_mode) ? '/' : '>');
    wrefresh(ctx.input_type_win);

    post_form(ctx.input_form);
    wrefresh(ctx.input_win);
}

void console_init(void)
{
    initscr();
    noecho();
    cbreak();

    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    WINDOW *dbg_win = newwin(maxY - 1, maxX / 2, 0, 0);
    scrollable_init(&ctx.dbg_win, dbg_win);

    WINDOW *chat_win = newwin(maxY - 1, maxX / 2, 0, maxX / 2);
    scrollable_init(&ctx.chat_win, chat_win);

    ctx.input_type_win = newwin(1, 1, maxY - 1, 0);
    cctx_set_cmd_mode(false);

    ctx.input_win = newwin(1, maxX - 2, maxY - 1, 2);
    keypad(ctx.input_win, true);

    ctx.fields[0] = new_field(1, maxX - 2, 0, 0, 0, 0);
    ctx.fields[1] = NULL;
    ctx.input_form = new_form(ctx.fields);
    set_form_win(ctx.input_form, ctx.input_win);
    set_form_sub(ctx.input_form, ctx.input_win);

    cctx_redraw_all();
    ctx.running = true;
}
void console_free(void)
{
    unpost_form(ctx.input_form);
    free_form(ctx.input_form);
    free_field(ctx.fields[0]);

    scrollable_free(&ctx.dbg_win);
    scrollable_free(&ctx.chat_win);
    delwin(ctx.dbg_win.win);
    delwin(ctx.chat_win.win);
    delwin(ctx.input_type_win);
    delwin(ctx.input_win);

    endwin();
}
void console_debug(const char *msg)
{
    scrollable_push(&ctx.dbg_win, msg);
    form_driver(ctx.input_form, REQ_VALIDATION);    // move cursor to input form
}
void console_chat(const char *msg)
{
    scrollable_push(&ctx.chat_win, msg);
    form_driver(ctx.input_form, REQ_VALIDATION);    // move cursor to input form
}
void console_main(command_callback_t cmd_callback)
{
    int c;
    while(ctx.running && (c = wgetch(ctx.input_win))) {
        switch(c) {
        case KEY_RESIZE:
            cctx_redraw_all();
            break;
        case KEY_LEFT:
            form_driver(ctx.input_form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(ctx.input_form, REQ_NEXT_CHAR);
            break;

        case KEY_BACKSPACE:
        case 127:
            form_driver(ctx.input_form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(ctx.input_form, REQ_DEL_CHAR);
            break;

        case '\n': {
            form_driver(ctx.input_form, REQ_VALIDATION);
            char *data = trim_whitespace(field_buffer(ctx.fields[0], 0));
            if(ctx.cmd_mode) {
                if(strcmp(data, "exit") == 0) {
                    ctx.running = false;
                } else {
                    cmd_callback(data);
                }
            } else {
                scrollable_push(&ctx.chat_win, data);
            }
            form_driver(ctx.input_form, REQ_CLR_FIELD);
            form_driver(ctx.input_form, REQ_VALIDATION);

            cctx_set_cmd_mode(false);
            break;
        }

        case '/':
            if(!ctx.cmd_mode) {
                cctx_set_cmd_mode(true);
            } else {
                form_driver(ctx.input_form, c);
            }
            break;
        default:
            form_driver(ctx.input_form, c);
            break;
        }
        wrefresh(ctx.input_win);
    }
}
