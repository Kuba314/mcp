#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "packet_handler.h"
#include "unionstream.h"
#include "connection.h"
#include "packets.h"
#include "debug.h"

#include "auth.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"


char *g_player_username = "username";
char *g_player_password = "password";

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    // int sockfd = connect_to_server(HYPIXEL_IP, 25565);
    int sockfd = connect_to_server(LOCALHOST, 25565);
    if(sockfd < 0) {
        return 1;
    }

    unionstream_t *stream = stream_create(sockfd);
    send_Handshake(stream, 47, 2);
    // send_StatusRequest(stream);
    send_LoginStart(stream, "Techn0manCZ", strlen("Techn0manCZ"));

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
    if(err) {
        return err;
    }
    return 1;
}
