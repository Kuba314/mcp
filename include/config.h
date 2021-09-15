
extern char *g_mojang_username;
extern char *g_mojang_password;
extern char *g_username;

extern int g_verbosity;

int load_config(const char *filename);
void free_config();
