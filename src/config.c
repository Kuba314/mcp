#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

#define LINE_BUFFER_LENGTH 1024

char *g_mojang_username = NULL;
char *g_mojang_password = NULL;
char *g_username = NULL;

int g_verbosity = 0;

int load_config(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        error("config", "couldn't open file %s", filename);
        return 1;
    }

    static char **config_data[3] = {
        &g_mojang_username,
        &g_mojang_password,
        &g_username,
    };
    static const char *prefixes[3] = {
        "login=",
        "password=",
        "username=",
    };

    // loop over lines
    char line[LINE_BUFFER_LENGTH] = "";
    while(fgets(line, sizeof(line), fp)) {
        if(line[sizeof(line) - 2] != '\0') {
            error("config", "line too long");
            return 1;
        }
        size_t line_length = strlen(line);
        for(size_t i = 0; i < sizeof(config_data) / sizeof(*config_data); i++) {
            size_t prefix_length = strlen(prefixes[i]);
            if(strncmp(line, prefixes[i], prefix_length) == 0) {
                size_t data_length = line_length - prefix_length - 1;
                char **data = config_data[i];
                *data = malloc(data_length + 1);
                if(*data == NULL) {
                    alloc_error();
                    return 1;
                }
                strncpy(*data, line + prefix_length, data_length);
                (*data)[data_length] = '\0';
            }
        }
    }
    fclose(fp);
    debug("config", "config loaded from %s", filename);
    return 0;
}
void free_config() {
    if(g_mojang_username == NULL) {
        free(g_mojang_username);
    }
    if(g_mojang_password == NULL) {
        free(g_mojang_password);
    }
    if(g_username == NULL) {
        free(g_username);
    }
}
