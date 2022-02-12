#pragma once

#include <stdbool.h>

typedef int (*command_callback_t)(const char *);
typedef void (*chat_callback_t)(const char *);

void console_init(void);
void console_free(void);

bool console_is_running(void);
void console_debug(const char *fmt, ...);
void console_chat(const char *fmt, ...);

void console_main(command_callback_t, chat_callback_t);
