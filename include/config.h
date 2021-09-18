#pragma once

extern char *g_login;
extern char *g_password;
extern char *g_username;

extern int g_verbosity;

int load_config(const char *filename);
void free_config();
