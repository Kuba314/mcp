#include "packet_handler.h"
#include "packets.h"

#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/x509.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "auth.h"


unsigned char g_aes_key[16];
unsigned char g_aes_iv[16];

// encrypt aes_key and enc_token with server's rsa public key
static int auth_create_encrypted_pair(string_t *pubkey, string_t *auth_token,
                                      string_t **enc_aes_key,
                                      string_t **enc_token)
{
    void *pubkey_buff_tmp = pubkey->s;
    EVP_PKEY *evp_key =
        d2i_PUBKEY(NULL, (const unsigned char **) &pubkey->s, pubkey->length);
    if(evp_key == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // restore pubkey string, because d2i_PUBKEY moves it
    pubkey->s = pubkey_buff_tmp;

    RSA *rsa = EVP_PKEY_get1_RSA(evp_key);
    if(rsa == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    verbose("auth", "parsed server public key");

    // generate 16 random bytes
    RAND_bytes(g_aes_key, 16);
    memcpy(g_aes_iv, g_aes_key, 16);

    // encrypt aes_key
    *enc_aes_key = string_alloc(128);
    if(*enc_aes_key == NULL) {
        return 1;
    }
    RSA_public_encrypt(16, g_aes_key, (uint8_t *) (*enc_aes_key)->s, rsa,
                       RSA_PKCS1_PADDING);
    verbose("auth", "secret encrypted");

    // encrypt token
    *enc_token = string_alloc(128);
    if(*enc_token == NULL) {
        perror("enc_token string allocation");
        return 1;
    }
    RSA_public_encrypt(auth_token->length, (uint8_t *) auth_token->s,
                       (uint8_t *) (*enc_token)->s, rsa, RSA_PKCS1_PADDING);
    verbose("auth", "verify token encrypted");

    RSA_free(rsa);
    EVP_PKEY_free(evp_key);
    return 0;
}

static int init_stream_for_encryption(unionstream_t *stream)
{
    // init encryption
    stream->en_ctx = EVP_CIPHER_CTX_new();
    if(stream->en_ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return 1;
    }
    if(!EVP_EncryptInit(stream->en_ctx, EVP_aes_128_cfb8(), g_aes_key,
                        g_aes_iv)) {
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
    if(!EVP_DecryptInit(stream->de_ctx, EVP_aes_128_cfb8(), g_aes_key,
                        g_aes_iv)) {
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
    string_t *server_id = stream_read_string(stream);
    if(server_id == NULL)
        return 1;

    string_t *pubkey = stream_read_string(stream);
    if(pubkey == NULL)
        return 1;

    string_t *verify_token = stream_read_string(stream);
    if(verify_token == NULL)
        return 1;

    // auth with mojang api
    if(authenticate_with_mojang(server_id, pubkey)) {
        return 1;
    }

    string_t *enc_aes_key;
    string_t *enc_token;
    if(auth_create_encrypted_pair(pubkey, verify_token, &enc_aes_key,
                                  &enc_token)) {
        return 1;
    }

    // send rsa encrypted data back to server
    if(send_EncryptionResponse(stream, enc_aes_key, enc_token)) {
        return 1;
    }

    // enable encryption on unionstream
    if(init_stream_for_encryption(stream)) {
        return 1;
    }

    string_free(server_id);
    string_free(pubkey);
    string_free(verify_token);
    string_free(enc_aes_key);
    string_free(enc_token);
    return 0;
}
