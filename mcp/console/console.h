#pragma once

#include <stdbool.h>

typedef void (*command_callback_t)(const char *);

void console_init(void);
void console_free(void);

bool console_is_running(void);
void console_debug(const char *fmt, ...);
void console_chat(const char *fmt, ...);

void console_main(command_callback_t);
