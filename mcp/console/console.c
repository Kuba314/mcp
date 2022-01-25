#include "console.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <ncurses.h>
#include <form.h>

#include "sw_buffer.h"

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

    wresize(ctx.dbg_win.win, maxY - 2, maxX / 2);
    mvwin(ctx.dbg_win.win, 0, 0);

    wresize(ctx.chat_win.win, maxY - 2, (maxX - 1) / 2);
    mvwin(ctx.chat_win.win, 0, maxX / 2 + 1);

    wresize(ctx.input_type_win, 1, 1);
    mvwin(ctx.input_type_win, maxY - 1, 0);

    wresize(ctx.input_win, 1, maxX - 2);
    mvwin(ctx.input_win, maxY - 1, 2);

    erase();
    mvvline(0, maxX / 2, ACS_VLINE, maxY - 2);
    mvhline(maxY - 2, 0, ACS_HLINE, maxX);
    mvaddch(maxY - 2, maxX / 2, ACS_BTEE);
    refresh();

    scrollable_redraw(&ctx.dbg_win);
    scrollable_redraw(&ctx.chat_win);

    mvwaddch(ctx.input_type_win, 0, 0, (ctx.cmd_mode) ? '/' : '>');
    wrefresh(ctx.input_type_win);

    post_form(ctx.input_form);
    wrefresh(ctx.input_win);
}

static void init_colors()
{
    if(!has_colors() || !can_change_color()) {
        return;
    }
    start_color();
    init_color(0x14, 666, 0, 0);        // §4 dark_red
    init_color(0x1c, 1000, 333, 333);   // §c red
    init_color(0x16, 1000, 666, 0);     // §6 gold
    init_color(0x1e, 1000, 1000, 333);  // §e yellow
    init_color(0x12, 0, 666, 0);        // §2 dark_green
    init_color(0x1a, 333, 1000, 333);   // §a green
    init_color(0x1b, 333, 1000, 1000);  // §b aqua
    init_color(0x13, 0, 666, 666);      // §3 dark_aqua
    init_color(0x11, 0, 0, 666);        // §1 dark_blue
    init_color(0x19, 333, 333, 1000);   // §9 blue
    init_color(0x1d, 1000, 333, 1000);  // §d light_purple
    init_color(0x15, 666, 0, 666);      // §5 dark_purple
    init_color(0x1f, 1000, 1000, 1000); // §f white
    init_color(0x17, 666, 666, 666);    // §7 gray
    init_color(0x18, 333, 333, 333);    // §8 dark_gray
    init_color(0x10, 0, 0, 0);          // §0 black

    init_pair(0x14, 0x14, COLOR_BLACK);
    init_pair(0x1c, 0x1c, COLOR_BLACK);
    init_pair(0x16, 0x16, COLOR_BLACK);
    init_pair(0x1e, 0x1e, COLOR_BLACK);
    init_pair(0x12, 0x12, COLOR_BLACK);
    init_pair(0x1a, 0x1a, COLOR_BLACK);
    init_pair(0x1b, 0x1b, COLOR_BLACK);
    init_pair(0x13, 0x13, COLOR_BLACK);
    init_pair(0x11, 0x11, COLOR_BLACK);
    init_pair(0x19, 0x19, COLOR_BLACK);
    init_pair(0x1d, 0x1d, COLOR_BLACK);
    init_pair(0x15, 0x15, COLOR_BLACK);
    init_pair(0x1f, 0x1f, COLOR_BLACK);
    init_pair(0x17, 0x17, COLOR_BLACK);
    init_pair(0x18, 0x18, COLOR_BLACK);
    init_pair(0x10, 0x10, COLOR_BLACK);
}
void console_init(void)
{
    initscr();
    init_colors();
    noecho();
    cbreak();

    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    WINDOW *dbg_win = newwin(maxY - 2, maxX / 2, 0, 0);
    scrollable_init(&ctx.dbg_win, dbg_win);

    WINDOW *chat_win = newwin(maxY - 2, (maxX - 1) / 2, 0, (maxX - 1) / 2 + 1);
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

bool console_is_running(void) {
    return ctx.running;
}

#define BUFF_LENGTH 1024
void console_debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buff[BUFF_LENGTH];
    vsnprintf(buff, BUFF_LENGTH, fmt, args);
    va_end(args);

    scrollable_push(&ctx.dbg_win, buff, strlen(buff));
    scrollable_redraw(&ctx.dbg_win);
    wrefresh(ctx.input_win);        // focus input form again
}
void console_chat(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buff[BUFF_LENGTH];
    vsnprintf(buff, BUFF_LENGTH, fmt, args);
    va_end(args);

    scrollable_push(&ctx.chat_win, buff, strlen(buff));
    scrollable_redraw(&ctx.chat_win);
    wrefresh(ctx.input_win);        // focus input form again
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
        case 127: {
            int x, y;
            getyx(ctx.input_win, y, x);
            (void) y;
            if(x == 0 && ctx.cmd_mode) {
                cctx_set_cmd_mode(false);
            } else {
                form_driver(ctx.input_form, REQ_DEL_PREV);
            }
            break;
        }
        case KEY_DC:
            form_driver(ctx.input_form, REQ_DEL_CHAR);
            break;

        case 4: // ^D
            ctx.running = false;
            break;

        case '\n': {
            form_driver(ctx.input_form, REQ_VALIDATION);
            char *data = trim_whitespace(field_buffer(ctx.fields[0], 0));
            if(ctx.cmd_mode) {
                if(strcmp(data, "exit") == 0) {
                    ctx.running = false;
                } else if(strcmp(data, "stop") == 0) {
                    ctx.running = false;
                    fgetc(stdin);
                } else {
                    cmd_callback(data);
                }
            } else {
                scrollable_push(&ctx.chat_win, data, strlen(data));
            }
            form_driver(ctx.input_form, REQ_CLR_FIELD);
            form_driver(ctx.input_form, REQ_VALIDATION);

            cctx_set_cmd_mode(false);
            break;
        }

        case '/': {
            int x, y;
            getyx(ctx.input_win, y, x);
            (void) y;
            if(x == 0 && !ctx.cmd_mode) {
                cctx_set_cmd_mode(true);
            } else {
                form_driver(ctx.input_form, c);
            }
            break;
        }
        default:
            form_driver(ctx.input_form, c);
            break;
        }
        wrefresh(ctx.input_win);
    }
}
