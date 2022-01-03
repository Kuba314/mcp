#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

#include "packet_handler.h"
#include "unionstream.h"
#include "connection.h"
#include "packets.h"
#include "world.h"
#include "debug.h"

#include "config.h"
#include "auth.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"

#define VERSION_ID_1_8_9 47

static bool running = true;

void *run_main_loop(void *stream)
{
    while(running) {

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

void run_console(unionstream_t *stream)
{
    (void) stream;
    while(running) {
        char buff[1024];
        if(fgets(buff, 1024, stdin) == 0) {
            debug("console", "probably ctrl c");
            return;
        }
        if(strncmp(buff, "exit", 4) == 0) {
            running = false;
            return;
        } else if(strncmp(buff, "?", 1) == 0) {
            debug("console", "sending chat message");
            send_ChatMessage(stream, buff + 1, strlen(buff) - 1);
        }
    }
}
void on_interrupt(int signum)
{
    (void) signum;
    running = false;
}

int main(int argc, char *argv[])
{
    const char *config_filename = NULL;
    g_verbosity = 0;
    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-v") == 0) {
            g_verbosity += 1;
        } else if(strcmp(argv[i], "-vv") == 0) {
            g_verbosity += 2;
        } else if(strcmp(argv[i], "-vvv") == 0) {
            g_verbosity += 3;
        } else if(strcmp(argv[i], "-c") == 0) {
            if(i == argc - 1) {
                error("args", "-c needs an argument");
                return 1;
            }
            config_filename = argv[i + 1];
            i++;
        } else if(i >= 1) {
            error("args", "invalid option: \"%s\"", argv[i]);
            return 1;
        }
    }
    if(config_filename == NULL) {
        config_filename = "user.cfg";
    }

    if(load_config(config_filename)) {
        return 1;
    }

    if(world_init()) {
        return 1;
    }

    int sockfd = connect_to_server(HYPIXEL_IP, 25565);
    // int sockfd = connect_to_server(LOCALHOST, 25565);
    if(sockfd < 0) {
        return 1;
    }

    unionstream_t *stream = stream_create(sockfd);
    send_Handshake(stream, VERSION_ID_1_8_9, 2);
    send_LoginStart(stream, g_username, strlen(g_username));

    pthread_t thread;
    pthread_create(&thread, NULL, run_main_loop, stream);

    signal(SIGINT, on_interrupt);
    run_console(stream);

    pthread_join(thread, NULL);

    stream_free(stream);
    free_config();
    return 0;
}
