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
#include "_string.h"
#include "sockbuff.h"
#include "utils.h"
#include "config.h"
#include "debug.h"

#define POST_BUFF_SIZE 1024

int mojang_authenticate(const char *username, const char *password, string_t **client_token, string_t **access_token,
                        string_t **uuid)
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

    sockbuff_t *data = sockbuff_create();
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

    verbose_begin("auth", "mojang authserver: ");
    for(size_t i = 0; i < data->length; i++) {
        verbose_frag("%c", data->data[i]);
    }
    verbose_end();

    if(extract_json_string_pair(data->data, data->length, "\"clientToken\"", client_token)) {
        return 1;
    }
    verbose("auth", "extracted clientToken: \"%s\"", (*client_token)->s);

    if(extract_json_string_pair(data->data, data->length, "\"accessToken\"", access_token)) {
        return 1;
    }
    verbose("auth", "extracted accessToken: \"%s\"", (*access_token)->s);

    if(extract_json_string_pair(data->data, data->length, "\"id\"", uuid)) {
        return 1;
    }
    verbose("auth", "extracted player uuid: \"%s\"", (*uuid)->s);

    sockbuff_free(data);
    return 0;
}
int mojang_join(string_t *client_token, string_t *access_token, string_t *player_uuid, string_t *server_id)
{
    CURL *curl = curl_easy_init();
    if(curl == NULL) {
        error("curl", "failed to initialize");
        return 1;
    }

    // format data buffer
    char post_buffer[POST_BUFF_SIZE + 1];
    int n_bytes = snprintf(post_buffer, POST_BUFF_SIZE,
                           "{\"clientToken\": \"%s\", \"accessToken\": \"%s\", \"selectedProfile\": "
                           "\"%s\", \"serverId\": \"%s\"}",
                           client_token->s, access_token->s, player_uuid->s, server_id->s);
    if(n_bytes == POST_BUFF_SIZE) {
        error("auth", "json username/password buffer too long");
        return 1;
    }

    verbose("auth", "sending post data to https://sessionserver.mojang.com/session/minecraft/join: %s", post_buffer);

    sockbuff_t *data = sockbuff_create();
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
        verbose_begin("auth", "mojang sessionserver: HTTP %ld ", http_code);
        for(size_t i = 0; i < data->length; i++) {
            verbose_frag("%c", data->data[i]);
        }
        verbose_end();
    }

    sockbuff_free(data);
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

    verbose_begin("auth", "sha1: ");
    verbose_string(hash_hex, hash_length);
    verbose_end();

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
    string_free(client_token);
    string_free(access_token);
    string_free(uuid);
    free(hash_hex);

    return 0;
}
