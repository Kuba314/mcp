#include <stdlib.h>
#include <stdbool.h>

#include "packet_handler.h"
#include "unionstream.h"
#include "connection.h"
#include "packets.h"
#include "debug.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"

int perform_handshake(unionstream_t *stream, int32_t proto_version,
                      const char *username)
{
    (void) username;

    send_Handshake(stream, proto_version, 2);
    // send_StatusRequest(sockfd);
    send_LoginStart(stream, username);

    // auth

    return 0;
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    int sockfd = connect_to_server(HYPIXEL_IP, 25565);
    // int sockfd = connect_to_server(LOCALHOST, 25565);
    if(sockfd < 0) {
        return 1;
    }

    unionstream_t stream = {
        .sockfd = sockfd,

    };
    int ret = perform_handshake(&stream, 47, "Techn0manCZ");
    (void) ret;

    int err = 0;
    while(true) {

        // load packet into memory
        if(stream_load_packet(&stream)) {
            err = 1;
            break;
        }

        // read packet id
        int32_t packet_id;
        if(stream_read_varint(&stream, &packet_id)) {
            err = 1;
            break;
        }

        if(handle_packet(packet_id, &stream)) {
            err = 1;
            break;
        }

        free(stream.data);
        stream.data = NULL;
    }
    stream_free(&stream);
    if(err) {
        return err;
    }
    return 1;
}
