#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

#include "console/console.h"
#include "net/packet_handler.h"
#include "net/stream.h"
#include "net/connection.h"
#include "net/packets.h"
#include "data/world.h"
#include "debug.h"

#include "config.h"
#include "auth/auth.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"

#define VERSION_ID_1_8_9 47

static bool running = true;
static stream_t *stream;

void *run_main_loop(void *stream)
{
    while(console_is_running() || (!g_console_enabled && running)) {

        // load packet into memory
        if(stream_load_packet(stream)) {
            break;
        }

        // read packet id
        int32_t packet_id;
        if(stream_read_varint(stream, &packet_id)) {
            break;
        }

        if(handle_packet(packet_id, stream)) {
            break;
        }

        stream_free_data(stream);
    }
    debug("thread", "handler exit");
    running = false;
    return NULL;
}

int cmd_callback(const char *cmd)
{
    if(strcmp(cmd, "exit") == 0) {
        running = false;
        return 1;
    } else if(strcmp(cmd, "respawn") == 0) {
        send_ClientStatus(stream, ACTION_ID_RESPAWN);
        info("play", "respawning...");
    } else {
        error("cmd", "Unknown command: \"%s\"", cmd);
    }
    return 0;
}
void chat_callback(const char *msg)
{
    send_ChatMessage(stream, msg);
}

void run_console(void)
{
    console_main(cmd_callback, chat_callback);
}
void run_without_console(void)
{
    char buff[1024 + 1];
    while(running) {
        if(fgets(buff, 1024, stdin) == 0) {
            debug("console", "probably ctrl c");
            running = false;
            return;
        }

        size_t buff_len = strlen(buff);
        if(buff_len == 0) {
            continue;
        } else if(buff[buff_len - 1] == '\n') {
            buff[buff_len - 1] = '\0';
        }

        if(buff[0] == '?' && buff[1]) {
            if(cmd_callback(buff + 1)) {
                return;
            }
        } else {
            chat_callback(buff);
        }
    }
}

int main(int argc, char *argv[])
{
    const char *config_filename = NULL;
    const char *server = NULL;
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-v") == 0) {
            g_verbosity -= 10;
        } else if(strcmp(argv[i], "-vv") == 0) {
            g_verbosity -= 20;
        } else if(strcmp(argv[i], "-vvv") == 0) {
            g_verbosity -= 30;
        } else if(strcmp(argv[i], "--console") == 0) {
            g_console_enabled = true;
        } else if(strcmp(argv[i], "-c") == 0) {
            if(i == argc - 1) {
                error("args", "-c needs an argument");
                return 1;
            }
            config_filename = argv[++i];
        } else if(strcmp(argv[i], "-s") == 0) {
            if(i == argc - 1) {
                error("args", "-s needs an argument");
                return 1;
            }
            server = argv[++i];
        } else if(i >= 1) {
            error("args", "invalid option: \"%s\"", argv[i]);
            return 1;
        }
    }
    if(server == NULL) {
        error("args", "need to have a server address set (-s)");
        return 1;
    }
    if(config_filename == NULL) {
        config_filename = "user.cfg";
    }

    if(g_console_enabled) {
        console_init();
    }
    if(load_config(config_filename)) {
        return 1;
    }

    if(world_init()) {
        return 1;
    }

    const uint16_t port = 25565;

    // int sockfd = connect_to_server(HYPIXEL_IP, 25565);
    int sockfd = connect_to_server(server, port);
    if(sockfd < 0) {
        return 1;
    }

    if(stream_create(sockfd, &stream)) {
        return 1;
    }
    send_Handshake(stream, VERSION_ID_1_8_9, "mc.hypixel.net", port, CONN_STATE_LOGIN);
    send_LoginStart(stream, g_username);

    pthread_t thread;
    pthread_create(&thread, NULL, run_main_loop, stream);

    // signal(SIGINT, on_interrupt);
    if(g_console_enabled) {
        run_console();
    } else {
        run_without_console();
    }

    pthread_join(thread, NULL);
    console_free();

    stream_free(stream);
    world_free();
    free_config();
    return 0;
}
