#include "auth.h"
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#include <curl/curl.h>

#include <openssl/sha.h>
#include "utils/dynstring.h"
#include "utils/buffer.h"
#include "utils/json.h"
#include "config.h"
#include "debug.h"

#define POST_BUFF_SIZE 1024

static char int2hex(char c)
{
    return c + ((c < 10) ? '0' : 'a' - 10);
}

static uint8_t *sha_mc_hexlify(unsigned char *hash, size_t length, size_t *out_length)
{

    // negative hash and null byte
    uint8_t *hash_hex = malloc(length * 2 + 2);
    if(hash_hex == NULL) {
        alloc_error();
        return NULL;
    }

    // space out the chars, count zeros
    int n_zeros = 0;
    bool nonzero_found = false;
    for(size_t i = 0; i < length; i++) {
        hash_hex[i * 2] = hash[i] >> 4;
        hash_hex[i * 2 + 1] = hash[i] & 0xf;

        if(hash_hex[i * 2] == 0 && !nonzero_found) {
            n_zeros++;
            if(hash_hex[i * 2 + 1] == 0) {
                n_zeros++;
                continue;
            }
        }
        nonzero_found = true;
    }

    int sign = hash[0] >> 7;
    if(sign) {
        int carry = 0;
        for(int i = 2 * length - 1; i >= 0; i--) {

            unsigned char new_c = (hash_hex[i] ^ 0xf) + carry;
            if(i == (int) length * 2 - 1) {
                new_c++;
            }
            carry = new_c >> 4;
            hash_hex[i] = new_c & 0xf;
        }
    }

    // move correctly
    if(sign || n_zeros) {
        memmove(hash_hex + sign, hash_hex + n_zeros, length * 2 - n_zeros);
        if(sign) {
            hash_hex[0] = '-';
        }
    }

    // convert to readable format
    for(int i = sign; i < (int) length * 2 + sign - n_zeros; i++) {
        hash_hex[i] = int2hex(hash_hex[i]);
    }

    *out_length = length * 2 + sign - n_zeros;
    hash_hex[*out_length] = '\0';
    return hash_hex;
}

// used as a curl callback
static size_t bufwrite(void *ptr, size_t size, size_t nmemb, buffer_t *buff)
{
    buffer_write(buff, ptr, size * nmemb);
    return size * nmemb;
}

int mojang_authenticate(const char *username, const char *password, string_t **client_token,
                        string_t **access_token, string_t **uuid)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) {
        error("curl", "failed to initialize");
        return 1;
    }

    // format data buffer
    char post_buffer[POST_BUFF_SIZE + 1];
    int n_bytes = snprintf(post_buffer, POST_BUFF_SIZE,
                           "{\"username\": \"%s\", \"password\": \"%s\", \"agent\": "
                           "{\"name\": \"Minecraft\", \"version\": 1}}",
                           username, password);
    if(n_bytes == POST_BUFF_SIZE) {
        error("auth", "json username/password buffer too long");
        return 1;
    }

    buffer_t *data = buffer_create();
    if(data == NULL) {
        return 1;
    }

    // create headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "https://authserver.mojang.com/authenticate");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bufwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        error("auth", "invalid error code on auth: %s", curl_easy_strerror(res));
        return 1;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    verbose("auth", "mojang authserver: %.*s", data->length, data->data);

    json_value *parsed = json_parse(data->data, data->length);
    if(parsed == NULL || parsed->type != json_object) {
        error("json", "invalid response type");
        return 1;
    }
    if(json_extract_string(parsed, "clientToken", client_token)) {
        return 1;
    }
    verbose("auth", "extracted clientToken: \"%s\"", (*client_token)->s);

    if(json_extract_string(parsed, "accessToken", access_token)) {
        return 1;
    }
    verbose("auth", "extracted accessToken: \"%s\"", (*access_token)->s);

    if(json_extract_string(parsed, "selectedProfile.id", uuid)) {
        return 1;
    }
    verbose("auth", "extracted player uuid: \"%s\"", (*uuid)->s);

    json_value_free(parsed);
    buffer_free(data);
    return 0;
}
int mojang_join(string_t *client_token, string_t *access_token, string_t *player_uuid,
                string_t *server_id)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) {
        error("curl", "failed to initialize");
        return 1;
    }

    // format data buffer
    char post_buffer[POST_BUFF_SIZE + 1];
    int n_bytes =
        snprintf(post_buffer, POST_BUFF_SIZE,
                 "{\"clientToken\": \"%s\", \"accessToken\": \"%s\", \"selectedProfile\": "
                 "\"%s\", \"serverId\": \"%s\"}",
                 client_token->s, access_token->s, player_uuid->s, server_id->s);
    if(n_bytes == POST_BUFF_SIZE) {
        error("auth", "json username/password buffer too long");
        return 1;
    }

    verbose("auth",
            "sending post data to https://sessionserver.mojang.com/session/minecraft/join: %s",
            post_buffer);

    buffer_t *data = buffer_create();
    if(data == NULL) {
        return 1;
    }

    // create headers
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, "https://sessionserver.mojang.com/session/minecraft/join");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bufwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        error("auth", "invalid error code on auth: %s", curl_easy_strerror(res));
        return 1;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if(http_code != 204) {
        verbose("auth", "mojang sessionserver: HTTP %ld %.*s", http_code, data->length, data->data);
    }

    buffer_free(data);
    return 0;
}

extern unsigned char g_aes_iv[16];

int authenticate_with_mojang(string_t *server_id, string_t *pubkey)
{
    // create SHA1 hash
    SHA_CTX ctx;
    SHA1_Init(&ctx);

    SHA1_Update(&ctx, server_id->s, server_id->length);
    SHA1_Update(&ctx, g_aes_iv, 16);
    SHA1_Update(&ctx, pubkey->s, pubkey->length);

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1_Final(hash, &ctx);

    size_t hash_length;
    uint8_t *hash_hex = sha_mc_hexlify(hash, SHA_DIGEST_LENGTH, &hash_length);
    if(hash_hex == NULL) {
        return 1;
    }

    verbose("auth", "sha1: \"%.*s\"", hash_length, hash_hex);

    string_t *server_id_hash = string_create((char *) hash_hex, hash_length);

    string_t *client_token;
    string_t *access_token;
    string_t *uuid;
    if(mojang_authenticate(g_login, g_password, &client_token, &access_token, &uuid)) {
        return 1;
    }
    if(mojang_join(client_token, access_token, uuid, server_id_hash)) {
        return 1;
    }
    string_free(server_id_hash);
    string_free(client_token);
    string_free(access_token);
    string_free(uuid);
    free(hash_hex);

    return 0;
}
