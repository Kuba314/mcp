#pragma once

#include <stddef.h>

typedef struct hashmap_item hashmap_item_t;
struct hashmap_item {
    hashmap_item_t *next;
    char *key;
    void *data;
};

typedef struct {
    size_t size;
    size_t bucket_count;
    hashmap_item_t *buckets[];
} hashmap_t;

hashmap_t *hashmap_create(size_t bucket_count);
void hashmap_free(hashmap_t *hashmap);

int hashmap_put(hashmap_t *hashmap, const char *key, void *data);
void *hashmap_remove(hashmap_t *hashmap, const char *key);
void *hashmap_find(hashmap_t *hashmap, const char *key);
