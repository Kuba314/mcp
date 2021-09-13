#pragma once

#include <stdint.h>

// handshake
int send_Handshake(int sockfd, int32_t proto_version, uint8_t next_state);

// status
int send_StatusRequest(int sockfd);

// login
int send_LoginStart(int sockfd, const char *username);
int send_EncryptionResponse(int sockfd, size_t secret_length,
                            const uint8_t *secret, size_t token_length,
                            const uint8_t *token);
