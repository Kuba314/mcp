#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <ncurses.h>
#include <form.h>

typedef struct {
    WINDOW *input_type_win;
    WINDOW *input_win;
    WINDOW *chat_win;
    WINDOW *dbg_win;
    FORM *input_form;
    FIELD *fields[2];
    bool cmd_mode;

    bool running;
} cctx_t;

void cctx_set_cmd_mode(cctx_t *ctx, bool cmd_mode)
{
    ctx->cmd_mode = cmd_mode;
    mvwaddch(ctx->input_type_win, 0, 0, (cmd_mode) ? '/' : '>');
    wrefresh(ctx->input_type_win);
}

void cctx_init(cctx_t *ctx)
{
    int maxX, maxY;
    getmaxyx(stdscr, maxY, maxX);

    ctx->dbg_win = newwin(maxY - 1, maxX / 2, 0, 0);
    box(ctx->dbg_win, 0, 0);
    wrefresh(ctx->dbg_win);

    ctx->chat_win = newwin(maxY - 1, maxX / 2, 0, maxX / 2);
    box(ctx->chat_win, 0, 0);
    wrefresh(ctx->chat_win);

    ctx->input_type_win = newwin(1, 1, maxY - 1, 0);
    cctx_set_cmd_mode(ctx, false);

    ctx->input_win = newwin(1, maxX - 2, maxY - 1, 2);
    keypad(ctx->input_win, true);

    ctx->fields[0] = new_field(1, maxX - 2, 0, 0, 0, 0);
    ctx->fields[1] = NULL;
    ctx->input_form = new_form(ctx->fields);
    set_form_win(ctx->input_form, ctx->input_win);
    set_form_sub(ctx->input_form, ctx->input_win);
    post_form(ctx->input_form);
    wrefresh(ctx->input_win);

    ctx->running = true;
}

void cctx_free(cctx_t *ctx)
{
    unpost_form(ctx->input_form);
    free_form(ctx->input_form);
    free_field(ctx->fields[0]);

    delwin(ctx->input_type_win);
    delwin(ctx->input_win);
    delwin(ctx->chat_win);
    delwin(ctx->dbg_win);
}

void cctx_execute_command(cctx_t *ctx, const char *cmd)
{
    if(strcmp(cmd, "exit") == 0) {
        ctx->running = false;
    }
}

void cctx_chat_append(cctx_t *ctx, const char *msg)
{
    (void) ctx;
    (void) msg;
}

char *trim_whitespace(char *data)
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

void cctx_main(cctx_t *ctx)
{
    int c;
    while(ctx->running && (c = wgetch(ctx->input_win))) {
        switch(c) {
        case KEY_LEFT:
            form_driver(ctx->input_form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(ctx->input_form, REQ_NEXT_CHAR);
            break;

        case KEY_BACKSPACE:
        case 127:
            form_driver(ctx->input_form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(ctx->input_form, REQ_DEL_CHAR);
            break;

        case '\n': {
            form_driver(ctx->input_form, REQ_VALIDATION);
            char *data = trim_whitespace(field_buffer(ctx->fields[0], 0));
            if(ctx->cmd_mode) {
                cctx_execute_command(ctx, data);
            } else {
                cctx_chat_append(ctx, data);
            }
            form_driver(ctx->input_form, REQ_CLR_FIELD);

            cctx_set_cmd_mode(ctx, false);
            break;
        }

        case '/':
            if(!ctx->cmd_mode) {
                cctx_set_cmd_mode(ctx, true);
            } else {
                form_driver(ctx->input_form, c);
            }
            break;
        default:
            form_driver(ctx->input_form, c);
            break;
        }
        wrefresh(ctx->input_win);
    }
}

int main()
{
    initscr();
    noecho();
    cbreak();

    cctx_t ctx;
    cctx_init(&ctx);
    cctx_main(&ctx);
    cctx_free(&ctx);

    endwin();
    return 0;
}
