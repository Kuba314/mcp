#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "packet_handler.h"
#include "unionstream.h"
#include "connection.h"
#include "packets.h"
#include "debug.h"

#include "config.h"
#include "auth.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"



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
        }
    }
    if(config_filename == NULL) {
        config_filename = "user.cfg";
    }

    if(load_config(config_filename)) {
        return 1;
    }

    // int sockfd = connect_to_server(HYPIXEL_IP, 25565);
    int sockfd = connect_to_server(LOCALHOST, 25565);
    if(sockfd < 0) {
        return 1;
    }

    unionstream_t *stream = stream_create(sockfd);
    send_Handshake(stream, 47, 2);
    // send_StatusRequest(stream);
    send_LoginStart(stream, g_username, strlen(g_username));

    int err = 0;
    while(true) {

        // load packet into memory
        if(stream_load_packet(stream)) {
            err = 1;
            break;
        }

        // read packet id
        int32_t packet_id;
        if(stream_read_varint(stream, &packet_id)) {
            err = 1;
            break;
        }

        if(handle_packet(packet_id, stream)) {
            err = 1;
            break;
        }

        stream_free_data(stream);
    }
    stream_free(stream);
    free_config();
    if(err) {
        return err;
    }
    return 1;
}
