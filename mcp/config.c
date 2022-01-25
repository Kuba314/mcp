#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "debug.h"

#define LINE_BUFFER_LENGTH 1024

char *g_login = NULL;
char *g_password = NULL;
char *g_username = NULL;

int g_verbosity = 0;

struct config_option {
    const char *key;
    char **const dest;
};

static const struct config_option options[] = {
    {"login",     &g_login   },
    { "password", &g_password},
    { "username", &g_username},
};
int load_config(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        error("config", "couldn't open file %s", filename);
        return 1;
    }

    // loop over lines
    char line[LINE_BUFFER_LENGTH] = "";
    while(fgets(line, sizeof(line), fp)) {
        if(line[sizeof(line) - 2] != '\0') {
            error("config", "line too long");
            return 1;
        }

        // loop over defined config options and try to match them
        size_t line_length = strlen(line);
        for(size_t i = 0; i < sizeof(options) / sizeof(*options); i++) {
            size_t key_length = strlen(options[i].key);
            if(strncmp(line, options[i].key, key_length) == 0) {
                if(line[key_length] != '=') {
                    continue;
                }

                size_t data_length = line_length - key_length - 2;
                char **const data = options[i].dest;
                *data = malloc(data_length + 1);
                if(*data == NULL) {
                    alloc_error();
                    return 1;
                }
                strncpy(*data, line + key_length + 1, data_length);
                (*data)[data_length] = '\0';
            }
        }
    }
    fclose(fp);

    bool missing_option = false;
    for(size_t i = 0; i < sizeof(options) / sizeof(*options); i++) {
        if(*options[i].dest == NULL) {
            error("config", "option \"%s\" not set", options[i].key);
            missing_option = true;
        }
    }
    if(missing_option) {
        return 1;
    }
    debug("config", "config loaded from \"%s\"", filename);
    return 0;
}
void free_config()
{
    for(size_t i = 0; i < sizeof(options) / sizeof(*options); i++) {
        if(options[i].dest != NULL) {
            free(*options[i].dest);
        }
    }
}
