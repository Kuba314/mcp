#include <stdio.h>
#include <stdbool.h>

#include <unistd.h>
#include "varint.h"

#include "connection.h"
#include "packet.h"
#include "unionstream.h"

#define HYPIXEL_IP "172.65.234.205"
#define LOCALHOST "127.0.0.1"

int perform_handshake(int sockfd, int32_t proto_version, const char *username) {
    (void) username;

    send_Handshake(sockfd, proto_version, 2);
    // send_StatusRequest(sockfd);
    send_LoginStart(sockfd, username);

    // auth

    return 0;
}

int main(int argc, char *argv[]) {

    (void) argc;
    (void) argv;

    int sockfd = mc_connect(HYPIXEL_IP, 25565);
    // int sockfd = mc_connect(LOCALHOST, 25565);
    if(sockfd < 0) {
        return 1;
    }

    int ret = perform_handshake(sockfd, 47, "Techn0manCZ");
    (void) ret;

    while(true) {

        // read packet
        unionstream_t stream;
        int32_t packet_id;
        if(get_packet_stream(sockfd, false, &stream, &packet_id)) {
            return 1;
        }

        handle_packet(packet_id, &stream);

        // // dump packet
        // printf("last packet id: %d\n", packet_id);
        // unsigned char c;
        // for(size_t i = 0; i < stream.length; i++) {
        //     stream_read(stream, &c, 1);
        //     printf("%c", c);
        // }
        // printf("\n");

        stream_free(&stream);
    }

    return 1;
}
