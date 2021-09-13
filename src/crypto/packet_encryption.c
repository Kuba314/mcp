#include "packet_encryption.h"

#include <openssl/err.h>
#include <openssl/evp.h>


// int AES_encrypt(unsigned char *dst, const unsigned char *src, size_t length) {

//     if(!EVP_EncryptUpdate(ctx, dst, out_length, src, length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }

//     int tmp_length;
//     if(!EVP_EncryptFinal(ctx, dst + *out_length, &tmp_length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }
//     *out_length += tmp_length;
//     return 0;
// }


// int AES_decrypt(unsigned char *dst, const unsigned char *src, size_t length) {

//     int out_length;
//     if(!EVP_DecryptUpdate(ctx, dst, &out_length, src, length)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }

//     int tmp_length;
//     if(!EVP_DecryptFinal(ctx, dst + *out_length, &final_len)) {
//         ERR_print_errors_fp(stderr);
//         return 1;
//     }
//     *out_length += tmp_length;
//     return 0;
// }
