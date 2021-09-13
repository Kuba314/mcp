#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "unionstream.h"
#include "packets.h"
#include "_string.h"
#include "debug.h"

unsigned char aes_key[16];
unsigned char aes_iv[16];

static int auth_create_encrypted_pair(string_t pubkey, string_t auth_token,
                                      string_t *enc_aes_key,
                                      string_t *enc_token)
{

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
    verbose("auth", "parsed server public key");

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
    verbose("auth", "secret encrypted");

    enc_token->s = malloc(128);
    if(enc_token->s == NULL) {
        perror("enc_token string allocation");
        return 1;
    }
    enc_token->length = 128;
    RSA_public_encrypt(auth_token.length, auth_token.s, enc_token->s, rsa,
                       RSA_PKCS1_PADDING);
    verbose("auth", "verify token encrypted");

    RSA_free(rsa);
    EVP_PKEY_free(evp_key);
    return 0;
}

int init_stream_for_encryption(unionstream_t *stream)
{

    // init encryption
    stream->en_ctx = EVP_CIPHER_CTX_new();
    if(stream->en_ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    if(!EVP_EncryptInit(stream->en_ctx, EVP_aes_128_cfb8(), aes_key, aes_iv)) {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_cleanup(stream->en_ctx);
        return 1;
    }

    // init decryption
    stream->de_ctx = EVP_CIPHER_CTX_new();
    if(stream->de_ctx == NULL) {
        EVP_CIPHER_CTX_cleanup(stream->en_ctx);
        ERR_print_errors_fp(stderr);
        return 1;
    }
    if(!EVP_DecryptInit(stream->de_ctx, EVP_aes_128_cfb8(), aes_key, aes_iv)) {
        EVP_CIPHER_CTX_cleanup(stream->en_ctx);
        EVP_CIPHER_CTX_cleanup(stream->de_ctx);
        ERR_print_errors_fp(stderr);
        return 1;
    }

    stream->is_encrypted = true;
    debug("auth", "unionstream is now encrypted");
    return 0;
}

int on_encryption_request(unionstream_t *stream)
{

    string_t server_id;
    if(stream_read_string(stream, &server_id))
        return 1;
    free(server_id.s);

    string_t pubkey;
    if(stream_read_string(stream, &pubkey))
        return 1;

    string_t verify_token;
    if(stream_read_string(stream, &verify_token))
        return 1;

    string_t enc_aes_key;
    string_t enc_token;
    auth_create_encrypted_pair(pubkey, verify_token, &enc_aes_key, &enc_token);
    free(pubkey.s);
    free(verify_token.s);

    send_EncryptionResponse(stream, enc_aes_key.length, enc_aes_key.s,
                            enc_token.length, enc_token.s);

    init_stream_for_encryption(stream);

    free(enc_aes_key.s);
    free(enc_token.s);
    return 0;
}
