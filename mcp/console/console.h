#pragma once

typedef void (*command_callback_t)(const char *);

void console_init(void);
void console_debug(const char *fmt, ...);
void console_chat(const char *fmt, ...);
void console_main(command_callback_t);
void console_free(void);
