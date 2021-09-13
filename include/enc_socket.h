#pragma once

#include <openssl/evp.h>

typedef struct {
    int sockfd;
    EVP_CIPHER_CTX *en_ctx;
    EVP_CIPHER_CTX *de_ctx;
} enc_socket_t;

extern unsigned char aes_key[16];
extern unsigned char aes_iv[16];

enc_socket_t *enc_socket_init(int sockfd);
void enc_socket_free(enc_socket_t *enc_sock);

int enc_socket_read(enc_socket_t *enc_sock, uint8_t *buff, size_t length);
int enc_socket_write(enc_socket_t *enc_sock, const uint8_t *buff, size_t length);
