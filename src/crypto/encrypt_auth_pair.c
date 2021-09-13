#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <stdlib.h>
#include <string.h>

#include "_string.h"
#include "debug.h"

#include "enc_socket.h"

unsigned char aes_key[16];
unsigned char aes_iv[16];

int auth_create_encrypted_pair(string_t pubkey, string_t auth_token,
                               string_t *enc_aes_key, string_t *enc_token) {

    EVP_PKEY *evp_key =
        d2i_PUBKEY(NULL, (const unsigned char **) &pubkey.s, pubkey.length);
    if(evp_key == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    RSA *rsa = EVP_PKEY_get1_RSA(evp_key);
    if(rsa == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    debug("auth", "parsed server public key");

    // generate 16 random bytes
    RAND_bytes(aes_key, 16);
    memcpy(aes_iv, aes_key, 16);

    enc_aes_key->s = malloc(128);
    if(enc_aes_key->s == NULL) {
        perror("enc_aes_key string allocation");
        return 1;
    }
    enc_aes_key->length = 128;
    RSA_public_encrypt(16, aes_key, enc_aes_key->s, rsa, RSA_PKCS1_PADDING);
    debug("auth", "secret encrypted");

    enc_token->s = malloc(128);
    if(enc_token->s == NULL) {
        perror("enc_token string allocation");
        return 1;
    }
    enc_token->length = 128;
    RSA_public_encrypt(auth_token.length, auth_token.s, enc_token->s, rsa,
                       RSA_PKCS1_PADDING);
    debug("auth", "verify token encrypted");

    return 0;
}
