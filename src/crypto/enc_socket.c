#include "enc_socket.h"

#include <openssl/aes.h>
#include <openssl/err.h>
#include <stdlib.h>

#include <sys/socket.h>
#include "debug.h"

// aes_key and aes_iv have to be set
enc_socket_t *enc_socket_init(int sockfd) {
    enc_socket_t *enc_sock = malloc(sizeof(enc_socket_t));
    if(enc_sock == NULL) {
        return NULL;
    }

    enc_sock->sockfd = sockfd;
    enc_sock->en_ctx = EVP_CIPHER_CTX_new();
    if(enc_sock->en_ctx == NULL) {
        ERR_print_errors_fp(stderr);
        free(enc_sock);
        return NULL;
    }
    if(!EVP_EncryptInit(enc_sock->en_ctx, EVP_aes_128_cfb8(), aes_key, aes_iv)) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    enc_sock->de_ctx = EVP_CIPHER_CTX_new();
    if(enc_sock->de_ctx == NULL) {
        EVP_CIPHER_CTX_cleanup(enc_sock->en_ctx);
        ERR_print_errors_fp(stderr);
        free(enc_sock);
        return NULL;
    }
    if(!EVP_DecryptInit(enc_sock->de_ctx, EVP_aes_128_cfb8(), aes_key, aes_iv)) {
        ERR_print_errors_fp(stderr);
        return NULL;
    }

    return enc_sock;
}
// void enc_socket_free(enc_socket_t *enc_sock) {
//     EVP_CIPHER_CTX_cleanup(enc_sock->en_ctx);
//     EVP_CIPHER_CTX_cleanup(enc_sock->de_ctx);
//     free(enc_sock);
// }

// int enc_socket_read(enc_socket_t *enc_sock, uint8_t *buff, size_t length) {

//     uint8_t *enc_buff = malloc(length);
//     if(enc_buff == NULL) {
//         error("alloc", "enc_socket_read");
//         return 1;
//     }
//     ssize_t recved = recv(enc_sock->sockfd, enc_buff, length, MSG_WAITALL);
//     if(recved < 0) {
//         perror("enc_socket_read:recv");
//         return recved;
//     } else if((size_t) recved != length) {
//         error("recv", "enc_socket_read didn't read enough");
//         return 1;
//     }

//     int out_length;
//     if(!EVP_DecryptUpdate(enc_sock->de_ctx, buff, &out_length, enc_buff, length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }
//     verbose("enc_socket_dec", "out_length %d", out_length);


//     // int tmp_length;
//     // if(!EVP_DecryptFinal(ctx, dst + *out_length, &final_len)) {
//     //     ERR_print_errors_fp(stderr);
//     //     return 1;
//     // }
//     // *out_length += tmp_length;

//     return 0;
// }
// int enc_socket_write(enc_socket_t *enc_sock, const uint8_t *buff, size_t length) {

//     uint8_t *enc_buff = malloc(length);
//     if(enc_buff == NULL) {
//         error("alloc", "enc_socket_write");
//         return 1;
//     }

//     int out_length;
//     if(!EVP_EncryptUpdate(enc_sock->en_ctx, enc_buff, &out_length, buff, length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }
//     verbose("enc_socket_enc", "out_length %d", out_length);
//     send(enc_sock->sockfd, enc_buff, length, 0);

//     // int tmp_length;
//     // if(!EVP_EncryptFinal(ctx, dst + *out_length, &tmp_length)) {
//     //     ERR_print_errors_fp(stderr);
//     //     return 1;
//     // }
//     // *out_length += tmp_length;
//     return 0;

// }
