#include "utils/hashmap.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "debug.h"

static uint32_t hashmap_hash(const char *key)
{
    uint32_t h = 0;

    for(const uint8_t *p = (const uint8_t *) key; *p != '\0'; p++) {
        h = 65599 * h + *p;
    }
    return h;
}

hashmap_t *hashmap_create(size_t bucket_count)
{
    hashmap_t *hashmap = calloc(1, sizeof(hashmap_t) + bucket_count * sizeof(hashmap_item_t));
    if(hashmap == NULL) {
        alloc_error();
        return NULL;
    }
    hashmap->bucket_count = bucket_count;
    return hashmap;
}
void hashmap_free(hashmap_t *hashmap)
{
    for(size_t i = 0; i < hashmap->bucket_count; i++) {
        hashmap_item_t *curr = hashmap->buckets[i];
        hashmap_item_t *tmp;

        while(curr != NULL) {
            tmp = curr->next;
            free(curr->key);
            free(curr);
            curr = tmp;
        }
    }
    free(hashmap);
}
void hashmap_free_all(hashmap_t *hashmap)
{
    for(size_t i = 0; i < hashmap->bucket_count; i++) {
        hashmap_item_t *curr = hashmap->buckets[i];
        hashmap_item_t *tmp;

        while(curr != NULL) {
            tmp = curr->next;
            free(curr->data);
            free(curr->key);
            free(curr);
            curr = tmp;
        }
    }
    free(hashmap);
}
int hashmap_put(hashmap_t *hashmap, const char *key, void *data)
{
    uint32_t bucket_index = hashmap_hash(key) % hashmap->bucket_count;

    hashmap_item_t *curr = hashmap->buckets[bucket_index];
    hashmap_item_t *last = NULL;

    while(curr != NULL) {
        if(strcmp(curr->key, key) == 0) {
            error("hashmap", "already contains key \"%s\"", key);
            return 0;
        }

        last = curr;
        curr = curr->next;
    }

    hashmap_item_t *new_item = malloc(sizeof(hashmap_item_t));
    if(new_item == NULL) {
        alloc_error();
        return 1;
    }
    new_item->next = NULL;
    new_item->key = malloc(strlen(key) + 1);
    if(new_item->key == NULL) {
        free(new_item);
        alloc_error();
        return 1;
    }
    strcpy(new_item->key, key);
    new_item->data = data;

    if(last == NULL) {
        hashmap->buckets[bucket_index] = new_item;
    } else {
        last->next = new_item;
    }
    hashmap->size++;
    return 0;
}
void *hashmap_remove(hashmap_t *hashmap, const char *key)
{
    uint32_t bucket_index = hashmap_hash(key) % hashmap->bucket_count;

    hashmap_item_t *curr = hashmap->buckets[bucket_index];
    hashmap_item_t *last = NULL;

    while(curr != NULL) {
        if(strcmp(curr->key, key) == 0) {
            void *ret = curr->data;
            if(last == NULL) {
                hashmap->buckets[bucket_index] = curr->next;
            } else {
                last->next = curr->next;
            }
            free(curr->key);
            free(curr);
            hashmap->size--;
            return ret;
        }

        last = curr;
        curr = curr->next;
    }
    return NULL;
}
void *hashmap_find(hashmap_t *hashmap, const char *key)
{
    uint32_t bucket_index = hashmap_hash(key) % hashmap->bucket_count;

    hashmap_item_t *curr = hashmap->buckets[bucket_index];

    while(curr != NULL) {
        if(strcmp(curr->key, key) == 0) {
            return curr->data;
        }

        curr = curr->next;
    }
    return NULL;
}
