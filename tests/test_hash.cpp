#include "gtest/gtest.h"

extern "C" {
#include <stdlib.h>
#include <openssl/sha.h>

#include "utils/dynstring.h"
#include "utils/utils.h"
}

TEST(Hashes, Notch)
{
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, "Notch", 5);
    SHA1_Final(hash, &ctx);

    size_t hash_length;
    uint8_t *hash_hex = sha_mc_hexlify(hash, SHA_DIGEST_LENGTH, &hash_length);
    ASSERT_NE(hash_hex, (void *) NULL);

    const char *expected_hash = "4ed1f46bbe04bc756bcb17c0c7ce3e4632f06a48";
    printf("%s\n", hash_hex);
    printf("%s\n", expected_hash);
    EXPECT_EQ(strcmp((char *) hash_hex, expected_hash), 0);
    free(hash_hex);
}
TEST(Hashes, jeb_)
{
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, "jeb_", 4);
    SHA1_Final(hash, &ctx);

    size_t hash_length;
    uint8_t *hash_hex = sha_mc_hexlify(hash, SHA_DIGEST_LENGTH, &hash_length);
    ASSERT_NE(hash_hex, (void *) NULL);

    const char *expected_hash = "-7c9d5b0044c130109a5d7b5fb5c317c02b4e28c1";
    printf("%s\n", hash_hex);
    printf("%s\n", expected_hash);
    EXPECT_EQ(strcmp((char *) hash_hex, expected_hash), 0);
    free(hash_hex);
}
TEST(Hashes, simon)
{
    unsigned char hash[SHA_DIGEST_LENGTH];

    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, "simon", 5);
    SHA1_Final(hash, &ctx);

    size_t hash_length;
    uint8_t *hash_hex = sha_mc_hexlify(hash, SHA_DIGEST_LENGTH, &hash_length);
    ASSERT_NE(hash_hex, (void *) NULL);

    const char *expected_hash = "88e16a1019277b15d58faf0541e11910eb756f6";
    printf("%s\n", hash_hex);
    printf("%s\n", expected_hash);
    EXPECT_EQ(strcmp((char *) hash_hex, expected_hash), 0);
    free(hash_hex);
}
