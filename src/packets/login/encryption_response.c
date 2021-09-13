#include <string.h>

#include "_string.h"
#include "debug.h"
#include "sockbuff.h"

int send_EncryptionResponse(int sockfd, size_t secret_length,
                            const char *secret, size_t token_length,
                            const char *token) {

    sockbuff_t *buff = sockbuff_create();
    if(buff == 0) {
        return 1;
    }

    int err = 0;
    err |= sockbuff_write_varint(buff, 1);
    err |= sockbuff_write_string(buff, secret, secret_length);
    err |= sockbuff_write_string(buff, token, token_length);
    if(err) {
        return err;
    }

    sockbuff_dumpto(buff, sockfd);
    return 0;
}
